//
// Created by nbaiot@126.com on 2019/12/7.
//
#pragma once

#ifndef NBSDK_FFTW_H
#define NBSDK_FFTW_H

#include <fft.h>
#include <fftw3.h>

namespace nbsdk::math {

class FFTW : public FFT {

public:

  ~FFTW() override ;

  void Init(int sampleCount) override;

  void Execute(std::complex<double>** out, const int16_t* in) override;

  void Terminate() override;

private:
  int sample_count_{0};
  int out_size_{0};
  fftw_plan plan_;
  double* in_{nullptr};
  fftw_complex* out_{nullptr};
};

}


#endif //NBSDK_FFTW_H
