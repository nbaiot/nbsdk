//
// Created by nbaiot@126.com on 2019/10/12.
//
#pragma once

#ifndef NBSDK_BASE_EVENT_LOOP_NLOG_H
#define NBSDK_BASE_EVENT_LOOP_NLOG_H

#include <vector>
#include <thread>
#include <atomic>

#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>


namespace nbsdk::base {

class EventLoop {

public:
  explicit EventLoop(int threadCount);

  ~EventLoop();

  void Loop();

  void Exit();

  boost::asio::io_context& IOContext();

private:
  boost::asio::io_context ioc_;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
  std::vector<std::thread> threads_;
  int thread_count_;
  std::atomic_bool running_;
};

}

#endif // NBSDK_BASE_EVENT_LOOP_NLOG_H


