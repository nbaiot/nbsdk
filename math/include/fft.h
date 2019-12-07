//
// Created by nbaiot@126.com on 2019/12/7.
//
#pragma once

#ifndef NBSDK_FFT_H
#define NBSDK_FFT_H

#include <vector>
#include <complex>

namespace nbsdk::math {

class FFT {

public:
  virtual ~FFT() = default;

  virtual void Init(int sampleCount) = 0;

  ///TODO: use Buffer replace out, in
  virtual void Execute(std::complex<double>** out, const int16_t* in) = 0;

  virtual void Terminate() = 0;
};

class IFFT {
public:
  virtual ~IFFT() = default;

  virtual void Init(int sampleCount) = 0;

  ///TODO: use Buffer replace out, in
  virtual void Execute(double** out, const std::complex<double>* in) = 0;

  virtual void Terminate() = 0;
};

}

#endif //NBSDK_FFT_H
