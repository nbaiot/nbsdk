//
// Created by nbaiot@126.com on 2019/12/24.
//

#pragma once

#ifndef NBSDK_HTTP_CALL_H
#define NBSDK_HTTP_CALL_H

#include <functional>

#include <base/include/smart_pointer.h>

namespace nbsdk::net {

class HttpClient;
class HttpRequest;
class HttpResponse;

class HttpCall {

public:

  typedef std::function<void(const sp<HttpCall>& call, const sp<HttpResponse>& response)> OnResponseCallback;
  typedef std::function<void(const sp<HttpCall>& call, int errorCode, std::string reason)> OnFailureCallback;

  HttpCall() = delete;

  HttpCall(const sp<HttpClient>& client, sp<HttpRequest> request);

  sp<HttpRequest> Request();

  sp<HttpResponse> Execute();

  void Enqueue(OnResponseCallback responseCallback, OnFailureCallback failureCallback);

  void Cancel();

  bool IsExecuted();

  bool IsCanceled();

private:
  wp<HttpClient> client_;
  sp<HttpRequest> request_;
};

}

#endif //NBSDK_HTTP_CALL_H
