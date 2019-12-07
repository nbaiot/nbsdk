//
// Created by nbaiot@126.com on 2019/12/5.
//

#include <iostream>
#include <atomic>
#include <base/include/nlog.h>
#include <vector>

#include <math/include/gcc_phat.h>

using namespace nbsdk::base;
using namespace nbsdk::math;

void rotate(std::vector<int16_t>& out, const std::vector<int16_t>& in, int rot) {
  int n = in.size();
  for (unsigned int i=0; i<n ;i++) {
    unsigned int j = (i+rot)%n;
    out[j] = in[i];
  }
}

int main() {
  LOG_INFO() << ">>>>>>>>>>>>>>>test nbsdk nlog";

  sp<GCCPhat> gccPhat = std::make_shared<GCCPhat>(8192);
  int    true_delay = 110;  /* The "true" delay value. This is the answer we are looking for. */
  double amp = 1.0;       /* amplitude of the sine wave - arbitrary */
  int    samp_freq = 512;   /* frequency of the sine wave - arbitrary */

  double samp_per = 1.0/(double)samp_freq; /* sample period of the test signals */
  int    nsamps = 8192;                  /* number of samples in the test signals */
  std::vector<int16_t> siga;                   /* storage for the test signal (sine wave) */
  siga.resize(nsamps);
  std::vector<int16_t> sigb;                   /* shifted version of test signal */
  sigb.resize(nsamps);

  for (unsigned i=0;i<nsamps;i++) {
    double v = amp * std::sin((2.0 * M_PI * samp_per * i));
    v = float(INT16_MAX) * v;
    int16_t vi = (int16_t)v;
    siga[i] = vi;

  }

  rotate(sigb, siga, true_delay);

  int margin = 50;
  int distance = gccPhat->Execute(sigb.data(), siga.data(), 8192,  120);

  std::cout << "Delay: " << true_delay << "\n"
            << "Calculated Delay: " << distance << "\n";

  return 0;
}