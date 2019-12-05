//
// Created by nbaiot@126.com on 2019/12/5.
//

#pragma  once

#ifndef NBSDK_CRITICAL_SECTION_H
#define NBSDK_CRITICAL_SECTION_H

#include <mutex>

#include "macros.h"

namespace nbsdk::base {

class CriticalSection {

public:
  CriticalSection() = default;
  ~CriticalSection() = default;

  void Enter()const;
  bool TryEnter()const;
  void Leave()const;

private:
  mutable std::mutex mutex_;
  DISALLOW_COPY_AND_ASSIGN(CriticalSection);
};

class CritScope {
public:
  explicit CritScope(const CriticalSection* cs);
  ~CritScope();

private:
  const CriticalSection* cs_;
  DISALLOW_COPY_AND_ASSIGN(CritScope);
};

}

#endif // NBSDK_CRITICAL_SECTION_H
