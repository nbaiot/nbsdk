//
// Created by nbaiot@126.com on 2019/12/5.
//

#include <iostream>

#include <base/include/nlog.h>
#include <net/include/http_url.h>

#include <boost/beast/http.hpp>

using namespace nbsdk::net;
using namespace nbsdk::base;
namespace http = boost::beast::http;
int main() {
  http::request<http::empty_body> req_;
  HttpUrl httpUrl("https://blog.csdn.net/csdngkk/article/details/103682497?utm_source=bkzx_BWzd");
  LOG_INFO() << "path:" << httpUrl.EncodedPath();
  return 0;
}