//
// Created by nbaiot@126.com on 2019/12/24.
//

#include <http_request.h>

namespace nbsdk::net {

Builder::Builder() : method_("GET") {
}

Builder::Builder(const HttpRequest &request) {
  url_ = request.url_;
  protocol_ = request.protocol_;
  method_ = request.method_;
  body_ = request.body_;
  headers_ = request.headers_;
}

Builder::Builder(const sp<HttpRequest> &request) {
  url_ = request->url_;
  protocol_ = request->protocol_;
  method_ = request->method_;
  body_ = request->body_;
  headers_ = request->headers_;
}

}