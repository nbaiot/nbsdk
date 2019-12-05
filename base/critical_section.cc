//
// Created by nbaiot@126.com on 2019/12/5.
//

#include <critical_section.h>

namespace nbsdk::base {

void CriticalSection::Enter()const {
  mutex_.lock();
}

bool CriticalSection::TryEnter()const {
 return mutex_.try_lock();
}

void CriticalSection::Leave()const {
  mutex_.unlock();
}

CritScope::CritScope(const CriticalSection *cs):cs_(cs) {
  cs_->Enter();
}

CritScope::~CritScope() {
  cs_->Leave();
}
}
