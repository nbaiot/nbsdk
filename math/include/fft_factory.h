//
// Created by nbaiot@126.com on 2019/12/7.
//
#pragma once

#ifndef NBSDK_FFT_FACTORY_H
#define NBSDK_FFT_FACTORY_H

#include <base/include/smart_pointer.h>

#include "fft.h"


using namespace nbsdk::base;

namespace nbsdk::math {

class FTTFactory {
public:
  static sp<FFT> CreateFFT();

  static sp<IFFT> CreateIFFT();
};

}

#endif //NBSDK_FFT_FACTORY_H
