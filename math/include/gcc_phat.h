//
// Created by nbaiot@126.com on 2019/12/7.
//

#pragma once

#ifndef NBSDK_GCC_PHAT_H
#define NBSDK_GCC_PHAT_H

#include <cstdint>

#include <base/include/smart_pointer.h>

#include "fft.h"

using namespace nbsdk::base;

namespace nbsdk::math {

class GCCPhat {
public:
  explicit GCCPhat(int sampleCount);

  ~GCCPhat();

  /// TODO: use Buffer replace s1, s2
  int Execute(const int16_t* s1, const int16_t* s2, int len, int margin);


private:
  int sample_count_;
  sp<FFT> fft_;
  sp<IFFT> ifft_;
};

}

#endif //NBSDK_GCC_PHAT_H
