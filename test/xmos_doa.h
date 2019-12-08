//
// Created by genglei-cuan on 2019/8/21.
//

#ifndef SOUNDCOMPUTE_XMOS_DOA_H
#define SOUNDCOMPUTE_XMOS_DOA_H

#include <vector>

namespace xmos {

class XmosDoa {

public:
  static int currentAngle(void* channel1, void* channel2, void* channel3, void*channel4,
  		int len, uint32_t fs);
};

}

#endif //SOUNDCOMPUTE_XMOS_DOA_H
