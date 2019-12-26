//
// Created by nbaiot@126.com on 2019/12/24.
//

#pragma once

#ifndef NBSDK_HTTP_RESPONSE_H
#define NBSDK_HTTP_RESPONSE_H

#include <string>
#include <map>

#include <base/include/smart_pointer.h>
#include "http_defines.h"

namespace nbsdk::net {

class HttpRequest;

struct ResponseBody {

};

/// boost::beast::http::response
/// boost::beast::http::header

class HttpResponse {

public:
  class Builder {

  };

private:
  HttpProtocol protocol_;
  sp<HttpRequest> request_;
  int code_;
  std::string message_;
  std::map<std::string, std::string> headers_;
  ResponseBody body_;

  int64_t send_request_at_millis_;
  int64_t received_response_at_millis_;
};

}

#endif //NBSDK_HTTP_RESPONSE_H
