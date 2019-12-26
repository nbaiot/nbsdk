//
// Created by nbaiot@126.com on 2019/12/24.
//

#include <base/include/nlog.h>

#include <http_request.h>

using namespace nbsdk::base;

namespace nbsdk::net {

HttpRequest::Builder::Builder(const HttpRequest &request) {
  url_ = request.url_;
  method_ = request.method_;
  headers_ = request.headers_;
  body_ = request.body_;
}

HttpRequest::Builder::Builder(const sp<HttpRequest> &request) {
  url_ = request->url_;
  method_ = request->method_;
  headers_ = request->headers_;
  body_ = request->body_;
}

HttpRequest::Builder &HttpRequest::Builder::Url(const std::string &url) {
  if (url.empty()) {
    LOG_ERROR() << "url empty";
    return *this;
  }
  url_ = std::make_shared<HttpUrl>(url);
  return *this;
}

HttpRequest::Builder &HttpRequest::Builder::Url(const HttpUrl &url) {
  url_ = std::make_shared<HttpUrl>(url);
  return *this;
}

HttpRequest::Builder &HttpRequest::Builder::AddHeader(const std::string &name, const std::string &value) {
  if (name.empty()) {
    LOG_ERROR() << "name empty";
    return *this;
  }
  if (value.empty()) {
    LOG_ERROR() << "value empty";
    return *this;
  }
  headers_.emplace(std::make_pair(name, value));
  return *this;
}

HttpRequest::Builder &HttpRequest::Builder::RemoveHeader(const std::string &name) {
  if (name.empty()) {
    LOG_ERROR() << "name empty";
    return *this;
  }
  headers_.erase(name);
  return *this;
}

HttpRequest::Builder &HttpRequest::Builder::ClearHeaders() {
  headers_.clear();
  return *this;
}

HttpRequest::Builder &HttpRequest::Builder::Method(const std::string &method, const sp<RequestBody>& body) {
  if (method.empty()) {
    LOG_ERROR() << "method empty";
    return *this;
  }
  /// TODO: check body, when method need body
  method_ = method;
  body_ = body;
  return *this;
}

HttpRequest HttpRequest::Builder::Build() {
  if (!url_) {
    LOG_ERROR() << "url is null";
  }
  return HttpRequest(*this);
}

HttpRequest::HttpRequest(const HttpRequest::Builder &builder) {
  url_ = builder.url_;
  method_ = builder.method_;
  headers_ = builder.headers_;
  body_ = builder.body_;
}

std::string HttpRequest::ToString() const {
  std::ostringstream request;
  request << "Request{method=" << method_ << ", ";
  request << "url=" << ((url_ != nullptr) ? url_->Url() : "");
  request << "}";
  return request.str();
}

sp<HttpUrl> HttpRequest::Url() {
  return url_;
}

std::string HttpRequest::Method() {
  return method_;
}

std::string HttpRequest::Header(const std::string &name) {
  return headers_[name];
}

std::map<std::string, std::string> HttpRequest::Headers() {
  return headers_;
}

HttpRequest::Builder HttpRequest::NewBuilder() {
  return HttpRequest::Builder(*this);
}
}