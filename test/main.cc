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
  /*http::request<http::empty_body> req_;
  std::string u = "https://blog.csdn.net:888/csdngkk/article/details/103682497?a=123&a=999&utm_source=bkzx_BWzd#fsssssragment-test";
  HttpUrl httpUrl(u);
  LOG_INFO() << "path:" << httpUrl.EncodedPath();
  LOG_INFO() << "path:" << u.substr(8);
  HttpUrl::Builder builder;
  LOG_INFO() << "u:" << u;
  builder.Parse(u);
  HttpUrl url(builder);
  LOG_INFO() << url.Scheme();
  LOG_INFO() << url.Host();
  LOG_INFO() << url.Port();
  LOG_INFO() << url.EncodedPath();
  LOG_INFO() << url.EncodedQuery();
  LOG_INFO() << url.EncodedFragment();
  LOG_INFO() << url.Url();*/

  std::map<std::string, std::string> test;
  test.emplace(std::make_pair("abc", "123"));
  test.emplace(std::make_pair("abc", "678"));
  LOG_INFO() << "size:" << test.size();
  for (auto& i: test) {
    LOG_INFO() << i.first << ":" << i.second;
  }
  http::string_body::value_type body;
  http::request<http::string_body > req;
  req.body() = std::move(body);
  req.set(http::field::user_agent, "");

  return 0;
}