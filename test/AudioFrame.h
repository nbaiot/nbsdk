
#pragma once

#include "Frame.h"

namespace sound
{

template<typename T>
using AudioFrame = Frame<T>;

using AudioFrame8 = AudioFrame<int8_t>;
using AudioFrame16 = AudioFrame<int16_t>;

}   //!< namespace sound
