//
// Created by nbaiot@126.com on 2019/12/5.
//
#pragma once

#ifndef NBSDK_NLOG_H
#define NBSDK_NLOG_H

#include <string>
#include <sstream>

#include "macros.h"

namespace nbsdk::base {

typedef int LogSeverity;

const LogSeverity LOG_S_INFO = 0;
const LogSeverity LOG_S_WARNING = 1;
const LogSeverity LOG_S_ERROR = 2;
const LogSeverity LOG_S_FATAL = 3;
const LogSeverity LOG_S_NUM_SEVERITIES = 4;

class Nlog {

public:
  Nlog(const char *file, int line, LogSeverity severity);

  Nlog(const char* file, int line, const char* condition);

  ~Nlog();

  std::ostream& stream() { return stream_; }

private:
  void Init(const char *file, int line);

private:
  const char *file_;
  int32_t line_;
  LogSeverity severity_;
  std::ostringstream stream_;

private:
  DISALLOW_COPY_AND_ASSIGN(Nlog);
};

class LogVoidify {
public:
  LogVoidify() = default;
  void operator&(std::ostream&) { }
};

#define LOG_INFO() Nlog(__FILE__, __LINE__, LOG_S_INFO).stream()

#define LOG_INFO_IF(condition) \
  !(condition) ? (void) 0 : LogVoidify() & LOG_INFO()

#define LOG_WARNING() Nlog(__FILE__, __LINE__, LOG_S_WARNING).stream()

#define LOG_WARNING_IF(condition) \
  !(condition) ? (void) 0 : LogVoidify() & LOG_WARNING()

#define LOG_ERROR() Nlog(__FILE__, __LINE__, LOG_S_ERROR).stream()

#define LOG_ERROR_IF(condition) \
  !(condition) ? (void) 0 : LogVoidify() & LOG_ERROR()

#define LOG_FATAL() Nlog(__FILE__, __LINE__, LOG_S_FATAL).stream()

#define LOG_FATAL_IF(condition) \
  !(condition) ? (void) 0 : LogVoidify() & LOG_FATAL()

#define CHECK(condition) \
  (condition) ? (void) 0 : LogVoidify() & Nlog(__FILE__, __LINE__, #condition).stream()


}

#endif //NBSDK_NLOG_H
