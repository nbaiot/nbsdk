//
// Created by nbaiot@126.com on 2019/12/7.
//

#include "fftw.h"

namespace nbsdk::math {

FFTW::~FFTW() {
  Terminate();
}

void FFTW::Init(int sampleCount) {
  sample_count_ = sampleCount;
  in_ = (double*)fftw_malloc(sizeof(double) * sample_count_);
  out_size_ = (sample_count_ / 2) + 1;
  out_ = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * out_size_);
  plan_ = fftw_plan_dft_r2c_1d(sample_count_, in_, out_, FFTW_ESTIMATE);
}

void FFTW::Execute(std::complex<double>** out, const int16_t* in) {
  if (out == nullptr || *out == nullptr || in == nullptr)
    return;
  /// TODO: fixme
  for (int i = 0; i < sample_count_; i++) {
    in_[i] = (double)in[i];
  }
  fftw_execute(plan_);
  std::complex<double>* oo = *out;
  for (int i = 0; i < out_size_; i++) {
    oo[i] = std::complex<double>((double)out_[i][0], (double)out_[i][1]);
  }
}

void FFTW::Terminate() {
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