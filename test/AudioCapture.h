
#pragma once

#include <functional>

namespace sound
{

template<typename F>
class AudioCapture {
public:

  typedef F FrameType;
  typedef std::function<void(const sp<FrameType>& frame)> FrameCallback;
  typedef std::function<void(int, std::string)> ErrorCallback;

  ~AudioCapture() {}

  virtual bool Open() = 0;

  virtual bool Start() = 0;

  virtual void Stop() = 0;

  virtual void Close() = 0;

  virtual void Loop() = 0;

  virtual void SetFrameCallback(FrameCallback callback) = 0;

  virtual void SetErrorCallback(ErrorCallback callback) = 0;

  virtual void SetFrameSize(int size) = 0;

  virtual void Snapshot(int frames) = 0;

  virtual int sample_rate() const = 0;

  virtual int sample_bits() const = 0;

  virtual int sample_channel() const = 0;

  virtual int unit_size() const = 0;

  virtual bool good() const = 0;
  
};   //!< class AudioCapture
  
}   //!< namespace sound
