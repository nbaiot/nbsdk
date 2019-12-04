//
// Created by nbaiot@126.com on 2019/12/5.
//

#pragma once

#ifndef NBSDK_PLATFORM_H
#define NBSDK_PLATFORM_H


#include <cstdint>

#if defined(OS_UNIX) || defined(OS_ANDROID)
#include <sys/types.h>
#endif

namespace nbsdk::base {

/// TODO: add others
#if defined(OS_UNIX) || defined(OS_ANDROID)
typedef pid_t ProcessId;
typedef pid_t ThreadId;
#endif

ProcessId GetCurrentProcId();

ThreadId GetCurrentThreadId();

}

#endif //NBSDK_PLATFORM_H
