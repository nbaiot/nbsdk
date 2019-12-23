//
// Created by nbaiot@126.com on 2019/10/12.
//

#include "base/include/event_loop.h"

namespace nbsdk::base {


EventLoop::EventLoop(int threadCount)
    : work_guard_(ioc_.get_executor()), thread_count_(threadCount), running_(false) {
  threads_.reserve(static_cast<unsigned long>(thread_count_ - 1));
}

EventLoop::~EventLoop() {
  Exit();
}

void EventLoop::Loop() {
  if (running_)
    return;
  running_ = true;
  ioc_.reset();
  for (auto i = thread_count_; i > 0; --i) {
    threads_.emplace_back(
        [this] {
            ioc_.run();
        });
  }
}

void EventLoop::Exit() {
  if (!running_)
    return;
  if (!ioc_.stopped()) {
    ioc_.stop();
  }
  for (std::thread& thread: threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  threads_.clear();
  running_ = false;
}

boost::asio::io_context &EventLoop::IOContext() {
  return ioc_;
}

}