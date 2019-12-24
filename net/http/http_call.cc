//
// Created by nbaiot@126.com on 2019/12/24.
//

#include <http_call.h>

#include <utility>

namespace nbsdk::net {

HttpCall::HttpCall(const sp<HttpClient>& client, sp<HttpRequest> request) : client_(client), request_(std::move(request)) {
}

sp<HttpRequest> HttpCall::Request() {
  return request_;
}

sp<HttpResponse> HttpCall::Execute() {
  return nbsdk::sp<HttpResponse>();
}

void HttpCall::Enqueue(HttpCall::OnResponseCallback responseCallback, HttpCall::OnFailureCallback failureCallback) {

}

void HttpCall::Cancel() {

}

bool HttpCall::IsExecuted() {
  return false;
}

bool HttpCall::IsCanceled() {
  return false;
}

}