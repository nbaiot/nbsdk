//
// Created by nbaiot@126.com on 2019/12/24.
//
#pragma once

#ifndef NBSDK_HTTP_CLIENT_H
#define NBSDK_HTTP_CLIENT_H

#include <boost/asio/ip/tcp.hpp>

namespace nbsdk::net {

class HttpClient {

private:
  bool retry_on_connection_failure_;
  int connect_timeout_ms_;
  int read_timeout_ms_;
  int write_timeout_ms_;
  int ping_interval_ms_;

  boost::asio::ip::tcp::resolver resolver_;
  boost::asio::ip::tcp::endpoint endpoint_;
  boost::asio::ip::tcp::socket socket_;
};

}

#endif //NBSDK_HTTP_CLIENT_H
