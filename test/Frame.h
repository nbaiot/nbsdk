
#pragma once

#include <iostream>

namespace sound
{

template<typename T>
class Frame {
public:

  typedef T UnitType;

  Frame(long seq, int size)
      : seq_(seq), size_(size) {
    if (size > 0) {
      data_ = calloc(1, size);
    } else {
      data_ = nullptr;
    }
  }

  virtual ~Frame() {
    if (data_) {
      free(data_);
    }
  }

  virtual long seq() const {
    return seq_;
  }

  virtual void* data() {
    return data_;
  }
    
  virtual const void* data() const {
    return data_;
  }
  
  virtual int size() const {
    return size_;
  }

  virtual int units() const {
    return size_ / sizeof(T);
  }

  virtual UnitType* unit(int idx) {
    return (T*)((int8_t*)data_ + idx * sizeof(T));
  }

  virtual const UnitType* unit(int idx) const {
    return (T*)((int8_t*)data_ + idx * sizeof(T));
  }
  
private:
  long          seq_{-1};
  int           size_{0};
  void*         data_{nullptr};
  
};   //!< class Frame


}   //!< namespace sound
