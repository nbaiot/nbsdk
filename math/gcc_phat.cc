//
// Created by shajia on 2019/12/7.
//

#include <base/include/nlog.h>
#include <cfloat>
#include <algorithm>

#include "gcc_phat.h"
#include "fft_factory.h"

namespace nbsdk::math {

GCCPhat::GCCPhat(int sampleCount) : sample_count_(sampleCount) {
  fft_ = FTTFactory::CreateFFT();
  fft_->Init(sample_count_);

  ifft_ = FTTFactory::CreateIFFT();
  ifft_->Init(sample_count_);
}

GCCPhat::~GCCPhat() {
  LOG_INFO() << "~~~~~~~~~~~~~~~~~GCCPhat";
};

int GCCPhat::Execute(const int16_t *s1, const int16_t *s2, int count, int margin) {
  if (s1 == nullptr || s2 == nullptr)
    return -1;
  CHECK(count == sample_count_);

  std::vector<std::complex<double>> s1FFT;
  s1FFT.resize(count);
  auto pS1FFT = s1FFT.data();
  fft_->Execute(&pS1FFT, s1);

  std::vector<std::complex<double>> s2FFT;
  s2FFT.resize(count);
  auto pS2FFT = s2FFT.data();
  fft_->Execute(&pS2FFT, s2);

  std::vector<std::complex<double>> R;
  R.resize(count);
  for (int i = 0; i < count; i++) {
    std::complex<double> v = s2FFT[i] * std::conj(s1FFT[i]);
    v = v / (std::abs(v) + FLT_MIN);
    R[i] = v;
  }

  std::vector<double> crossCorrelation;
  crossCorrelation.resize(count);
  auto pCC = crossCorrelation.data();
  ifft_->Execute(&pCC, R.data());


  std::vector<double> shifted;
  //shifted.resize(crossCorrelation.size());
  auto half = std::round(crossCorrelation.size() / 2.0);
  auto half_iter = crossCorrelation.begin() + half;
  std::copy(half_iter, crossCorrelation.end(), std::back_inserter(shifted));
  std::copy(crossCorrelation.begin(), half_iter, std::back_inserter(shifted));

  auto newMargin = margin;
  if ((half - newMargin) < 0) {
    newMargin = half;
  }
  if ((half + newMargin) >= sample_count_) {
    newMargin = (sample_count_ - 1) - half;
  }

  auto start = half - newMargin;
  auto len = 2 * newMargin + 1;

  int maxIndex = std::distance(shifted.begin() + start,
      std::max_element(shifted.begin() + start, shifted.begin() + start + len));

  int argMax = maxIndex - newMargin;

  return argMax;
}

}