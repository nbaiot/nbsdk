//
// Created by genglei-cuan on 2019/CHANNEL_COUNT/20.
//

#include "xmos_frame.h"

#define CHANNEL_COUNT 5

namespace xmos {

XmosFrame::XmosFrame(long seq, int size) : Frame(seq, size) {
  for (int i=0; i<CHANNEL_COUNT; ++i) {
    channel_frames_.push_back(std::make_shared<AudioFrame16>(seq, size / CHANNEL_COUNT));
  }
}

XmosFrame::~XmosFrame() = default;

void *XmosFrame::separationChannel() {
  int peer = CHANNEL_COUNT * 2;
  for (int i = 0; i < (size() / peer); ++i) {
    for (int j = 0; j < CHANNEL_COUNT; ++j) {
      ((int16_t*)channel_frames_[j]->data())[i] = ((int16_t*)data())[i * CHANNEL_COUNT + j];
    }
  }
}


long XmosFrame::seq() const {
  return Frame::seq();
}

void *XmosFrame::data() {
  return Frame::data();
}

const void *XmosFrame::data() const {
  return Frame::data();
}

int XmosFrame::size() const {
  return Frame::size();
}

int XmosFrame::units() const {
  return Frame::units();
}

short *XmosFrame::unit(int idx) {
  return Frame::unit(idx);
}

const short *XmosFrame::unit(int idx) const {
  return Frame::unit(idx);
}

std::shared_ptr<AudioFrame16> XmosFrame::channelData(int index) {
  if (index >= CHANNEL_COUNT)
    return nullptr;
  return channel_frames_[index];
}

}