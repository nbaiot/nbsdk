//
// Created by nbaiot@126.com on 2019/12/5.
//
#pragma once

#ifndef NBSDK_SMART_POINTER_H
#define NBSDK_SMART_POINTER_H

#include <memory>

namespace nbsdk::base {

template<typename T>
using sp = std::shared_ptr<T>;

template<typename T>
using wp = std::weak_ptr<T>;


}

#endif //NBSDK_SMART_POINTER_H
