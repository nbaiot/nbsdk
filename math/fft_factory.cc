//
// Created by nbaiot@126.com on 2019/12/7.
//

#include <fft_factory.h>
#include "fftw.h"
#include "ifftw.h"

namespace nbsdk::math {


sp<FFT> FTTFactory::CreateFFT() {
  return std::make_shared<FFTW>();
}

sp<IFFT> FTTFactory::CreateIFFT() {
  return std::make_shared<IFFTW>();
}
}