//
// Created by nbaiot@126.com on 2019/12/7.
//
#pragma once

#ifndef NBSDK_IFFTW_H
#define NBSDK_IFFTW_H

#include <fft.h>
#include <fftw3.h>

namespace nbsdk::math {

class IFFTW : public IFFT {

public:
  ~IFFTW() override;

  void Init(int sampleCount) override;

  void Execute(double** out, const std::complex<double>* in) override;

  void Terminate() override;

private:
  int in_size_{0};
  int out_size_{0};
  double* out_{nullptr};
  fftw_complex* in_{nullptr};
  fftw_plan plan_;

};


}

#endif //NBSDK_IFFTW_H
