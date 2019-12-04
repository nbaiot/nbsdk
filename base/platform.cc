//
// Created by nbaiot@126.com on 2019/12/5.
//

#if defined(OS_UNIX) || defined(OS_ANDROID)
#include <unistd.h>
#include <syscall.h>
#endif

#include <platform.h>

namespace nbsdk::base {

ProcessId GetCurrentProcId() {
#if defined(OS_UNIX) || defined(OS_ANDROID)
  return getpid();
#endif
}

ThreadId GetCurrentThreadId() {
#if defined(OS_UNIX) || defined(OS_ANDROID)
  return syscall(__NR_gettid);
#endif
}
}