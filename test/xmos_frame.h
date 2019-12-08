//
// Created by genglei-cuan on 2019/8/20.
//

#ifndef SOUNDCOMPUTE_XMOS_FRAME_H
#define SOUNDCOMPUTE_XMOS_FRAME_H

#include <vector>

#include "../memery/include/buffer.h"

#include "AudioFrame.h"

using namespace nbsdk::memory;
using namespace sound;

namespace xmos {

class XmosFrame : public AudioFrame16 {
public:
  XmosFrame(long seq, int size);

  ~XmosFrame() override;

  long seq() const override;

  std::shared_ptr<AudioFrame16> channelData(int index);

  void* separationChannel();

  void *data() override;

  const void *data() const override;

  int size() const override;

  int units() const override;

  short *unit(int idx) override;

  const short *unit(int idx) const override;


private:
  std::vector<std::shared_ptr<AudioFrame16>> channel_frames_;
};

}

#endif //SOUNDCOMPUTE_XMOS_FRAME_H
