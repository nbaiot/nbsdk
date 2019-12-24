//
// Created by nbaiot@126.com on 2019/12/24.
//

#pragma once

#ifndef NBSDK_HTTP_REQUEST_H
#define NBSDK_HTTP_REQUEST_H

#include <string>
#include <map>

#include <base/include/smart_pointer.h>

#include "http_defines.h"

namespace nbsdk::net {

class HttpRequest;

struct RequestBody {

};

class Builder {

public:
  Builder();

  explicit Builder(const HttpRequest& request);

  explicit Builder(const sp<HttpRequest>& request);

  Builder& Url(const std::string& url);

  Builder&

private:
  HttpProtocol protocol_;
  std::string url_;
  std::string method_;
  std::map<std::string, std::string> headers_;
  RequestBody body_;
};

/**
 * request:
 * Request line
 * Request header
 * Request body
 */
class HttpRequest {

private:
  friend class Builder;
  HttpProtocol protocol_;
  std::string url_;
  std::string method_;
  std::map<std::string, std::string> headers_;
  RequestBody body_;
};

}

#endif //NBSDK_HTTP_REQUEST_H
