//
// Created by nbaiot@126.com on 2019/12/24.
//

#include <http_url.h>

#include <base/include/nlog.h>

using namespace nbsdk::base;

namespace nbsdk::net {

httpUrl::Builder &httpUrl::Builder::Scheme(const std::string &scheme) {
  if (scheme.empty()) {
    LOG_ERROR() << "scheme empty, use default scheme: http";
    scheme_ = "http";
  }
  if (scheme == "http")
    scheme_ = "http";
  else if (scheme == "https")
    scheme_ = "https";
  else {
    LOG_ERROR() << "unexpected scheme:" << scheme << ", use default scheme: http";
    scheme_ = "http";
  }
  return *this;
}

httpUrl::Builder &httpUrl::Builder::Host(const std::string &host) {
  if (host.empty()) {
    LOG_ERROR() << "host empty";
    return *this;
  }
  return *this;
}

httpUrl::Builder &httpUrl::Builder::Port(int port) {
  if (port <= 0 || port > 65535) {
    LOG_ERROR() << "unexpected port:" << port;
    port_ = -1;
  } else {
    port_ = port;
  }
  return *this;
}





int HttpUrl::DefaultPort(const std::string &scheme) {
  if (scheme == "http")
    return 80;
  else if (scheme == "https")
    return 443;
  return -1;
}
}