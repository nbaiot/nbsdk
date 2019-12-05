//
// Created by nbaiot@126.com on 2019/12/5.
//

#include <cstring>

#include <fifo_buffer.h>

namespace nbsdk::memory {

FifoBuffer::FifoBuffer(size_t length)
    : buffer_(new char[length]),
      buffer_length_(length),
      data_length_(0),
      read_position_(0) {

}

size_t FifoBuffer::GetDataLen() {
  std::lock_guard <std::mutex> lk(mutex_);
  return data_length_;
}

bool FifoBuffer::SetCapacity(size_t size) {
  std::lock_guard <std::mutex> lk(mutex_);
  if (data_length_ > size) {
    return false;
  }

  if (size != buffer_length_) {
    char *buffer = new char[size];
    const size_t copy = data_length_;
    const size_t tail_copy = std::min(copy, buffer_length_ - read_position_);
    memcpy(buffer, &buffer_[read_position_], tail_copy);
    memcpy(buffer + tail_copy, &buffer_[0], copy - tail_copy);
    buffer_.reset(buffer);
    read_position_ = 0;
    buffer_length_ = size;
  }
  return true;
}

size_t FifoBuffer::Read(void *buffer, size_t bytes) {
  std::lock_guard <std::mutex> lk(mutex_);
  size_t copy = 0;
  if (ReadOffsetLocked(buffer, bytes, 0, &copy) == 0) {
    read_position_ = (read_position_ + copy) % buffer_length_;
    data_length_ -= copy;
    return copy;
  }
  return copy;
}

size_t FifoBuffer::Write(const void *buffer, size_t bytes) {
  std::lock_guard <std::mutex> lk(mutex_);
  size_t copy = 0;
  if (WriteOffsetLocked(buffer, bytes, 0, &copy) == 0) {
    data_length_ += copy;
    return copy;
  }
  return copy;
}

int FifoBuffer::ReadOffsetLocked(void *buffer, size_t bytes, size_t offset, size_t *bytes_read) {
  if (offset >= data_length_) {
    return -1;
  }

  const size_t available = data_length_ - offset;
  const size_t read_position = (read_position_ + offset) % buffer_length_;
  const size_t copy = std::min(bytes, available);
  const size_t tail_copy = std::min(copy, buffer_length_ - read_position);
  char *const p = static_cast<char *>(buffer);
  memcpy(p, &buffer_[read_position], tail_copy);
  memcpy(p + tail_copy, &buffer_[0], copy - tail_copy);

  if (bytes_read) {
    *bytes_read = copy;
  }
  return 0;
}

int FifoBuffer::WriteOffsetLocked(const void *buffer, size_t bytes, size_t offset,
                                  size_t *bytes_written) {

  if (data_length_ + offset >= buffer_length_) {
    return -1;
  }

  const size_t available = buffer_length_ - data_length_ - offset;
  const size_t write_position =
      (read_position_ + data_length_ + offset) % buffer_length_;
  const size_t copy = std::min(bytes, available);
  const size_t tail_copy = std::min(copy, buffer_length_ - write_position);
  const char *const p = static_cast<const char *>(buffer);
  memcpy(&buffer_[write_position], p, tail_copy);
  memcpy(&buffer_[0], p + tail_copy, copy - tail_copy);

  if (bytes_written) {
    *bytes_written = copy;
  }
  return 0;
}

}