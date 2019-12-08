//
// Created by genglei-cuan on 2019/8/20.
//

#ifndef SOUNDCOMPUTE_XMOS_CAPTURE_H
#define SOUNDCOMPUTE_XMOS_CAPTURE_H

#include <alsa/asoundlib.h>

#include <memory>
#include <string>
#include <atomic>
#include <thread>

#include "../memery/include/buffer.h"
#include "../memery/include/fifo_buffer.h"

#include "xmos_frame.h"
#include "AudioCapture.h"
#include "AudioFrame.h"

using namespace nbsdk::memory;
using namespace sound;

namespace xmos {

class XmosCapture : public AudioCapture<AudioFrame16>, public std::enable_shared_from_this<XmosCapture> {
public:

  explicit XmosCapture(const char* device, int channels, uint32_t sampleRate);

  bool Open() override;

  bool Start() override;

  void Stop() override;

  void Close() override;

  void Loop() override;

  void SetFrameCallback(FrameCallback callback) override;

  void SetErrorCallback(ErrorCallback callback) override;

  void SetFrameSize(int size) override;

  void Snapshot(int frames) override;

  int sample_rate() const override;

  int sample_bits() const override;

  int sample_channel() const override;

  int unit_size() const override;

  bool good() const override;

private:
  void ParseData();

private:
  snd_pcm_t *handle_{};
  std::string device_;
  int channels_;
  uint32_t sample_rate_;
  std::atomic<bool> started_{false};
  std::atomic<bool> exit_flag_{false};
  std::atomic<bool> occur_error_{false};
  std::unique_ptr<std::thread> cap_thread_{nullptr};
  Buffer buffer_;
  FrameCallback                 frame_cb_;
  ErrorCallback                 error_cb_;
  std::unique_ptr<FifoBuffer> receive_Buffer_{nullptr};
  int                           frame_size_{0};
  long                          seq_{0};
};

}

#endif //SOUNDCOMPUTE_XMOS_CAPTURE_H
