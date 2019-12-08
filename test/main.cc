//
// Created by nbaiot@126.com on 2019/12/5.
//

#include <iostream>
#include <atomic>
#include <base/include/nlog.h>
#include <vector>

#include <math/include/gcc_phat.h>

#include "xmos_capture.h"

using namespace nbsdk::base;
using namespace nbsdk::math;
using namespace xmos;

void rotate(std::vector<int16_t>& out, const std::vector<int16_t>& in, int rot) {
  int n = in.size();
  for (unsigned int i=0; i<n ;i++) {
    unsigned int j = (i+rot)%n;
    out[j] = in[i];
  }
}

void test() {
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

}

FILE* file = nullptr;
FILE* file1 = nullptr;
FILE* file2 = nullptr;
FILE* file3 = nullptr;
FILE* file4 = nullptr;
int main() {
  LOG_INFO() << ">>>>>>>>>>>>>>>test nbsdk nlog";
//  file = fopen("xmos0.pcm", "w+");
//  file1 = fopen("xmos1.pcm", "w+");
//  file2 = fopen("xmos2.pcm", "w+");
//  file3 = fopen("xmos3.pcm", "w+");
//  file4 = fopen("xmos4.pcm", "w+");

  float mis = 2;
  sp<GCCPhat> gccPhat = std::make_shared<GCCPhat>(160 * mis);
  auto xmosCapture = std::make_shared<XmosCapture>("plughw:1", 5, 16000);
  /// 200ms
  xmosCapture->SetFrameSize(2 * 160 * 5 * mis);
  xmosCapture->SetFrameCallback([gccPhat, mis](const sp<AudioFrame16>& frame){
//    LOG_INFO() << ">>>>>>>>>>>>>frame size:" << frame->size() / 5 / 2;
      auto da0 = std::dynamic_pointer_cast<XmosFrame>(frame)->channelData(0)->data();
      auto da1 = std::dynamic_pointer_cast<XmosFrame>(frame)->channelData(1)->data();
      auto da2 = std::dynamic_pointer_cast<XmosFrame>(frame)->channelData(2)->data();
      auto da3 = std::dynamic_pointer_cast<XmosFrame>(frame)->channelData(3)->data();
      auto da4 = std::dynamic_pointer_cast<XmosFrame>(frame)->channelData(4)->data();
//      fwrite(da0, 1, 4410 * 2, file);
//      fwrite(da1, 1, 4410 * 2, file1);
//      fwrite(da2, 1, 4410 * 2, file2);
//   `   fwrite(da3, 1, 4410 * 2, file3);
//      fwrite(da4, 1, 4410 * 2, file4);
      int distance12 = gccPhat->Execute((int16_t*)da1, (int16_t*)da2, frame->size() / 5 / 2,  10);
      int distance13 = gccPhat->Execute((int16_t*)da1, (int16_t*)da3, frame->size() / 5 / 2,  10);
      int distance14 = gccPhat->Execute((int16_t*)da1, (int16_t*)da4, frame->size() / 5 / 2,  10);
      LOG_INFO() << ">>>>>>>>>distance12:" << 100 * distance12 * 340 / 44100;
      LOG_INFO() << ">>>>>>>>>distance13:" << 100 *distance13 * 340 / 44100;
      LOG_INFO() << ">>>>>>>>>distance14:" << 100 *distance14* 340 / 44100;
      LOG_INFO() << "==========================================================";
  });
  xmosCapture->Open();
  xmosCapture->Start();
  while (1) {
    sleep(1);
  }
//  sleep(10);
//  fclose(file);
//  fclose(file1);
//  fclose(file2);
//  fclose(file3);
//  fclose(file4);
  return 0;
}