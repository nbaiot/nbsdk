//
// Created by nbaiot@126.com on 2019/12/5.
//
#pragma once

#ifndef NBSDK_BUFFER_H
#define NBSDK_BUFFER_H

#include <memory>
#include <cstring>

#include <base/include/macros.h>
#include <base/include/nlog.h>

using namespace nbsdk::base;

namespace nbsdk::memory {

/// 动态扩展和收缩的 buffer
/// CopyOnWriteBuffer
template <typename T>
class BufferT {
  /// T 必须是 POD 类型，即只有默认的构造和析构方法，同时没有虚方法
  static_assert(std::is_pod<T>::value, "T must be a trivial type.");
  static_assert(!std::is_const<T>::value, "T may not be const");

public:
  using value_type = T;

  /// 创建一个空 buffer
  BufferT():size_(0), capacity_(0), data_(nullptr) {}

	BufferT(const BufferT&) = delete;

	BufferT& operator=(const BufferT&) = delete;

  /// 从临时 buffer 创建
  BufferT(BufferT&& buf)
   : size_(buf.size()),
     capacity_(buf.capacity()),
     data_(std::move(buf.data_)) {
    CHECK(IsConsistent());
    buf.OnMovedFrom();
  }

  /// 创建已经分配空间 capacity_ 大小的 buffer
  BufferT(uint32_t size, uint32_t capacity)
   : size_(size),
     capacity_(std::max(size, capacity)),
     data_(capacity_ > 0 ? new T[capacity_] : nullptr) {
    CHECK(IsConsistent());
  }
  explicit BufferT(uint32_t size) : BufferT(size, size) {}

  /// 从数组中拷贝到 buffer
  BufferT(const T* data, uint32_t size, uint32_t capacity) : BufferT(size, capacity) {
    std::memcpy(data_.get(), data, size * sizeof(T));
  }
  BufferT(const T* data, uint32_t size) : BufferT(data, size, size) {}

  ~BufferT() = default;

  const T* data() const {
    CHECK(IsConsistent());
    return data_.get();
  }

  T* data() {
    CHECK(IsConsistent());
    return data_.get();
  }

  /// 是否空
  bool empty() const {
    CHECK(IsConsistent());
    return size_ == 0;
  }

  /// 当前已占用大小
  uint32_t size() const {
    CHECK(IsConsistent());
    return size_;
  }

  /// 已分配空间大小
  uint32_t capacity() const {
    CHECK(IsConsistent());
    return capacity_;
  }

  /// 右值赋值构造
  BufferT& operator=(BufferT&& buf) {
    CHECK(IsConsistent());
    CHECK(buf.IsConsistent());
    size_ = buf.size_;
    capacity_ = buf.capacity_;
    data_ = std::move(buf.data_);
    buf.OnMovedFrom();
    return *this;
  }

  /// buffer 比较
  bool operator==(const BufferT& buf) const {
    CHECK(IsConsistent());
    if (size_ != buf.size_) {
      return false;
    }
    if (std::is_integral<T>::value) {
      return std::memcmp(data_.get(), buf.data_.get(), size_ * sizeof(T)) == 0;
    }
    for (uint32_t i = 0; i < size_; ++i) {
      if (data_[i] != buf.data_[i]) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const BufferT& buf) const { return !(*this == buf); }

  T& operator[](uint32_t index) {
    CHECK(index < size_);
    return data()[index];
  }

  T operator[](uint32_t index) const {
    CHECK(index < size_);
    return data()[index];
  }

  /// 迭代器
  T* begin() { return data(); }
  T* end() { return data() + size(); }
  const T* begin() const { return data(); }
  const T* end() const { return data() + size(); }

  /// 设置数据
  void SetData(const T* data, uint32_t size) {
    CHECK(IsConsistent());
    size_ = 0;
    AppendData(data, size);
  }

  void SetData(const T& w) {
    SetData(w.data(), w.size());
  }

  /// 追加数据
  void AppendData(const T* data, uint32_t size) {
    CHECK(IsConsistent());
    const uint32_t new_size = size_ + size;
    EnsureCapacityWithHeadroom(new_size, true);
    std::memcpy(data_.get() + size_, data, size * sizeof(T));
    size_ = new_size;
    CHECK(IsConsistent());
  }

  void SetSize(uint32_t size) {
    EnsureCapacityWithHeadroom(size, true);
    size_ = size;
  }

  void EnsureCapacity(uint32_t capacity) {
    EnsureCapacityWithHeadroom(capacity, false);
  }

  void Clear() {
    size_ = 0;
    CHECK(IsConsistent());
  }


	friend void swap(BufferT& a, BufferT& b) {
    std::swap(a.size_, b.size_);
    std::swap(a.capacity_, b.capacity_);
    std::swap(a.data_, b.data_);
  }

private:

  bool IsConsistent() const {
    return (data_ || capacity_ == 0) && capacity_ >= size_;
  }

  void OnMovedFrom() {
    size_ = 0;
    capacity_ = 0;
  }

  void EnsureCapacityWithHeadroom(uint32_t capacity, bool extra_headroom) {
    CHECK(IsConsistent());
    if (capacity <= capacity_)
      return;

    /// extra_headroom 为 true，最少扩容到原来的 1.5 倍
    const uint32_t new_capacity =
     extra_headroom ? std::max(capacity, capacity_ + capacity_ / 2)
                    : capacity;

    std::unique_ptr<T[]> new_data(new T[new_capacity]);
    std::memcpy(new_data.get(), data_.get(), size_ * sizeof(T));
    data_ = std::move(new_data);
    capacity_ = new_capacity;
    CHECK(IsConsistent());
  }

private:
  uint32_t size_;
  uint32_t capacity_;
  std::unique_ptr<T[]> data_;
};

using Buffer = BufferT<uint8_t>;

}

#endif // NBSDK_BUFFER_H
