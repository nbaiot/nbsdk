//
// Created by nbaiot@126.com on 2019/12/7.
//

#include "ifftw.h"

namespace nbsdk::math {

IFFTW::~IFFTW() {
  Terminate();
}

void IFFTW::Init(int sampleCount) {
  if (out_size_ == sampleCount)
    return;
  Terminate();
  out_size_ = sampleCount;
  out_ = (double*)fftw_malloc(sizeof(double) * out_size_);
  in_size_ = (out_size_ / 2) + 1;
  in_ = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * in_size_);
  plan_ = fftw_plan_dft_c2r_1d(out_size_, in_, out_, FFTW_ESTIMATE);
}

void IFFTW::Execute(double** out, const std::complex<double>* in) {
  if (out == nullptr || *out == nullptr || in == nullptr)
    return;
  for(int i = 0; i < in_size_; i++) {
    in_[i][0] = (double)in[i].real();
    in_[i][1] = (double)in[i].imag();
  }
  fftw_execute(plan_);
  double* oo = *out;
  for (int i = 0; i < out_size_; i++) {
    oo[i] = out_[i];
  }
}

void IFFTW::Terminate() {
  fftw_destroy_plan(plan_);
  if (in_) {
    fftw_free(in_);
    in_ = nullptr;
  }
  if (out_) {
    fftw_free(out_);
    out_ = nullptr;
  }
}

}