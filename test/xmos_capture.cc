
#include <memory>
#include <thread>
#include <base/include/smart_pointer.h>
#include "xmos_capture.h"

#define ALSA_BUFFER_SIZE_MAX 131072

namespace xmos {

XmosCapture::XmosCapture(const char *device, int channels, uint32_t sampleRate)
: device_(device), channels_(channels), sample_rate_(sampleRate){
  receive_Buffer_ = std::make_unique<FifoBuffer>(1024 * 1024);
}

bool XmosCapture::Open() {

  if (handle_ != nullptr) {
    std::cerr << "xmos already opened";
    return false;
  }

  snd_pcm_uframes_t max_buffer_size;
  snd_pcm_uframes_t period_size;
  /// 1. 非阻塞模式打开 audio device
  int res = snd_pcm_open(&handle_, device_.c_str(), SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK);
  if (res < 0) {
    std::cerr << "cannot open audio device:" << device_ << ", error:" << snd_strerror(res) << std::endl;
    return false;
  }
  /// 2. 配置 audio device
  snd_pcm_hw_params_t *hw_params;
  res = snd_pcm_hw_params_malloc(&hw_params);
  if (res < 0) {
    std::cerr << "cannot allocate hardware parameter structure:" << snd_strerror(res) << std::endl;
    goto fail1;
  }
  res = snd_pcm_hw_params_any(handle_, hw_params);
  if (res < 0) {
    std::cerr << "cannot initialize hardware parameter structure:" << snd_strerror(res) << std::endl;
    goto fail;
  }
  /// 2.1 交错模式
  res = snd_pcm_hw_params_set_access(handle_, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (res < 0) {
    std::cerr << "cannot set access type:" << snd_strerror(res) << std::endl;
    goto fail;
  }

  /// 2.2 设置 format
  res = snd_pcm_hw_params_set_format(handle_, hw_params, SND_PCM_FORMAT_S16_LE);
  if (res < 0) {
    std::cerr << "cannot set sample format:" << snd_strerror(res) << std::endl;
    goto fail;
  }

  /// 2.3 设置采样率
  res = snd_pcm_hw_params_set_rate_near(handle_, hw_params, &sample_rate_, nullptr);
  if (res < 0) {
    std::cerr << "cannot set sample rate:" << sample_rate_ << ", error:" << snd_strerror(res) << std::endl;
    goto fail;
  }
  /// 2.4 设置通道数
  res = snd_pcm_hw_params_set_channels(handle_, hw_params, channels_);
  if (res < 0) {
    std::cerr << "cannot set channel:" << channels_ << ", error:" << snd_strerror(res) << std::endl;
    goto fail;
  }

  /// 2.5 设置 buffer size
  snd_pcm_hw_params_get_buffer_size_max(hw_params, &max_buffer_size);
  max_buffer_size = std::min<snd_pcm_uframes_t>(max_buffer_size, ALSA_BUFFER_SIZE_MAX);
  res = snd_pcm_hw_params_set_buffer_size_near(handle_, hw_params, &max_buffer_size);
  if (res < 0) {
    std::cerr << "cannot set ALSA buffer size, error:" << snd_strerror(res) << std::endl;
    goto fail;
  }
  /// 2.6 设置 period size
  snd_pcm_hw_params_get_period_size_min(hw_params, &period_size, nullptr);
  if (!period_size)
    period_size = max_buffer_size / 4;
  res = snd_pcm_hw_params_set_period_size_near(handle_, hw_params, &period_size, nullptr);
  if (res < 0) {
    std::cerr << "cannot set ALSA period size, error:" << snd_strerror(res) << std::endl;
    goto fail;
  }

  res = snd_pcm_hw_params(handle_, hw_params);
  if (res < 0) {
    std::cerr << "cannot set parameters, error:" << snd_strerror(res) << std::endl;
    goto fail;
  }
  snd_pcm_hw_params_free(hw_params);
  return true;
  fail:
  snd_pcm_hw_params_free(hw_params);
  fail1:
  snd_pcm_close(handle_);
  return false;
}

bool XmosCapture::Start() {
  if (started_) {
    std::cerr << "xmos already started." << std::endl;
    return false;
  }
  int res = snd_pcm_prepare(handle_);
  if (res < 0) {
    std::cerr << "snd_pcm_prepare failed, error:" << snd_strerror(res) << std::endl;
    return false;
  }
  res = snd_pcm_start(handle_);
  if (res < 0) {
    std::cerr << "snd_pcm_start failed, error:" << snd_strerror(res) << std::endl;
    return false;
  }
  cap_thread_ = std::make_unique<std::thread>(std::bind(&XmosCapture::Loop, shared_from_this()));
  started_ = true;
  return true;
}

void XmosCapture::Stop() {
  exit_flag_ = true;
  if (cap_thread_ && cap_thread_->joinable())
    cap_thread_->join();
}

void XmosCapture::Close() {
  Stop();
  if (handle_ != nullptr) {
    snd_pcm_nonblock(handle_, 0);
    snd_pcm_drain(handle_);
    snd_pcm_close(handle_);
    handle_ = nullptr;
    started_ = false;
  }
}

void XmosCapture::Loop() {
  while (!exit_flag_) {
    if (!started_)
      break;
    snd_pcm_sframes_t frames;
    snd_pcm_sframes_t avail_frames;
    /// 1. 获取可读 frames
    avail_frames = snd_pcm_avail_update(handle_);
    if (avail_frames < 0) {
      occur_error_ = true;
      if (error_cb_) {
        error_cb_(avail_frames, snd_strerror(avail_frames));
      }
      break;
    } else if (avail_frames == 0) {
      snd_pcm_wait(handle_, 5);
      continue;
    }
//    std::cout << "avail_frames:" << avail_frames<< std::endl;
    // 2. 读取 frames
    buffer_.Clear();
    buffer_.EnsureCapacity(static_cast<uint32_t>(snd_pcm_frames_to_bytes(handle_, avail_frames)));
    frames = snd_pcm_readi(handle_, buffer_.data(), avail_frames);
    if (frames == -EPIPE) {
      std::cout << "overrun occurred..." << std::endl;
      snd_pcm_prepare(handle_);
      snd_pcm_pause(handle_, 0);
      continue;
    } else if (frames < 0) {
      occur_error_ = true;
      if (error_cb_) {
        error_cb_(frames, snd_strerror(frames));
      }
      std::cout << "error:" << snd_strerror(frames)<< std::endl;
      break;
    }
    buffer_.SetSize(snd_pcm_frames_to_bytes(handle_, frames));
    receive_Buffer_->Write(buffer_.data(), buffer_.size());
    ParseData();
  }
  std::cout << ">>>>>>>>>>>>>>>>> xmos capture thread exit" << std::endl;
}

void XmosCapture::ParseData() {
  if (receive_Buffer_->GetDataLen() < frame_size_) {
    return;
  }
  sp<XmosFrame> frame(new XmosFrame(seq_++, frame_size_));
  receive_Buffer_->Read(frame->data(), frame_size_);
  frame->separationChannel();
  if (frame_cb_) {
    frame_cb_(frame);
  }
}

void XmosCapture::SetFrameCallback(AudioCapture<AudioFrame16>::FrameCallback callback) {
  frame_cb_ = std::move(callback);
}

void XmosCapture::SetErrorCallback(AudioCapture<AudioFrame16>::ErrorCallback callback) {
  error_cb_ = std::move(callback);
}

void XmosCapture::SetFrameSize(int size) {
  frame_size_ = size;
}

void XmosCapture::Snapshot(int frames) {

}

int XmosCapture::sample_rate() const {
  return sample_rate_;
}

int XmosCapture::sample_bits() const {
  return 16;
}

int XmosCapture::sample_channel() const {
  return channels_;
}

int XmosCapture::unit_size() const {
  return channels_ * 16 / 8;
}

bool XmosCapture::good() const {
  return started_ && !occur_error_;
}

}