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
  std::string u = "https://blog.csdn.net:888/csdngkk/article/details/103682497?utm_source=bkzx_BWzd#fragment-test";
  HttpUrl httpUrl(u);
  LOG_INFO() << "path:" << httpUrl.EncodedPath();
  LOG_INFO() << "path:" << u.substr(8);
  HttpUrl::Builder builder;
  LOG_INFO() << "u:" << u;
  builder.Parse(u);
  HttpUrl url(builder);
  LOG_INFO() << url.Host();
  LOG_INFO() << url.Port();
  LOG_INFO() << url.EncodedFragment();
  return 0;
}