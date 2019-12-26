//
// Created by nbaiot@126.com on 2019/12/24.
//

#pragma once

#ifndef NBSDK_HTTP_REQUEST_H
#define NBSDK_HTTP_REQUEST_H

#include <string>
#include <map>

#include <boost/beast/http.hpp>

#include <base/include/smart_pointer.h>
#include <http_url.h>

namespace nbsdk::net {

struct RequestBody {

};

/**
 * request:
 * Request line
 * Request header
 * Request body
 */
class HttpRequest {

public:
  class Builder {

  public:
    Builder() = default;

    explicit Builder(const HttpRequest& request);

    explicit Builder(const sp<HttpRequest>& request);

    Builder& Url(const std::string& url);

    Builder& Url(const HttpUrl& url);

    Builder& AddHeader(const std::string& name, const std::string& value);

    Builder& RemoveHeader(const std::string& name);

    Builder& ClearHeaders();

    Builder& Method(const std::string& method, const sp<RequestBody>& body);

    HttpRequest Build();

  private:
    friend class HttpRequest;
    sp<HttpUrl> url_;
    std::string method_;
    std::map<std::string, std::string> headers_;
    sp<RequestBody> body_;
  };

public:
  explicit HttpRequest(const Builder& builder);

  std::string ToString() const;

  sp<HttpUrl> Url();

  std::string Method();

  std::string Header(const std::string& name);

  std::map<std::string, std::string> Headers();

  Builder NewBuilder();

private:
  sp<HttpUrl> url_;
  std::string method_;
  std::map<std::string, std::string> headers_;
  sp<RequestBody> body_;
};

}

#endif //NBSDK_HTTP_REQUEST_H
