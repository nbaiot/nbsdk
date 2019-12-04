//
// Created by nbaiot@126.com on 2019/12/5.
//

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/stacktrace.hpp>

#include <nlog.h>
#include <platform.h>

#if defined(OS_ANDROID)
#include <android/log.h>
#define ANDROID_TAG "nbsdk"
#endif

namespace nbsdk::base {

const char* const log_severity_names[] = {"INFO", "WARNING", "ERROR", "FATAL"};

const char* log_severity_name(int severity) {
  if (severity >= 0 && severity < LOG_S_NUM_SEVERITIES)
    return log_severity_names[severity];
  return "UNKNOWN";
}

Nlog::Nlog(const char *file, int line, LogSeverity severity) : severity_(severity), file_(file), line_(line) {
  Init(file, line);
}

Nlog::Nlog(const char *file, int line, const char *condition)  : severity_(LOG_S_FATAL), file_(file), line_(line)  {
  Init(file, line);
  stream_ << "Check failed: " << condition << ". ";
}

void Nlog::Init(const char *file, int line) {
  stream_ << "[";
#if !defined(OS_ANDROID)
  /// format:<date time>
  boost::posix_time::ptime p = boost::posix_time::microsec_clock::local_time();
  auto date = p.date();
  std::ostringstream os;
  auto facet = new boost::gregorian::date_facet("%m-%d");
  os.imbue(std::locale(os.getloc(), facet));
  os << date << " " << boost::posix_time::to_simple_string_type<char>(p.time_of_day());
  stream_ << os.str() << " ";
  /// format <pid tid>
  stream_ << GetCurrentProcId() << " " << GetCurrentThreadId() << " ";
  /// log severity
  stream_ << log_severity_name(severity_) << " ";
#endif
  /// file name
  std::string fileName(file);
  auto pos = fileName.find_last_of("\\/");
  if (pos != std::string::npos) {
    fileName = fileName.substr(pos + 1);
  }
  stream_ << fileName << "(" << line << ")] ";

}

Nlog::~Nlog() {
  stream_ << std::endl;
  std::string str_newline(stream_.str());
#if defined(OS_ANDROID)
  android_LogPriority priority =
   (severity_ < 0) ? ANDROID_LOG_VERBOSE : ANDROID_LOG_UNKNOWN;
  switch (severity_) {
    case LOG_S_INFO:
      priority = ANDROID_LOG_INFO;
      break;
    case LOG_S_WARNING:
      priority = ANDROID_LOG_WARN;
      break;
    case LOG_S_ERROR:
      priority = ANDROID_LOG_ERROR;
      break;
    case LOG_S_FATAL:
      priority = ANDROID_LOG_FATAL;
      break;
  }
  __android_log_write(priority, ANDROID_TAG, str_newline.c_str());
#elif defined(OS_UNIX)
  switch (severity_) {
    case LOG_S_INFO:
    case LOG_S_WARNING:
      ::fwrite(str_newline.data(), str_newline.size(), 1, stdout);
      fflush(stdout);
      break;
    case LOG_S_ERROR:
    case LOG_S_FATAL:
      ::fwrite(str_newline.data(), str_newline.size(), 1, stderr);
      fflush(stderr);
      break;
  }
  /// TODO: fixme
  if (severity_ == LOG_S_FATAL) {
    auto ms = boost::stacktrace::to_string(boost::stacktrace::stacktrace());
    ::fwrite(ms.c_str(), ms.size(), 1, stderr);
    fflush(stderr);
    _exit(1);
  }
#endif

}

}