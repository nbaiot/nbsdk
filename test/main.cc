//
// Created by nbaiot@126.com on 2019/12/5.
//

#include <iostream>

#include <webrtc/modules/audio_processing/include/audio_processing.h>

int main() {
  auto apm = webrtc::AudioProcessing::Create();
  return 0;
}