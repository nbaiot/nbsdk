//
// Created by nbaiot@126.com on 2019/12/24.
//
#pragma once

#ifndef NBSDK_HTTP_URL_H
#define NBSDK_HTTP_URL_H

#include <string>
#include <list>
#include <map>

namespace nbsdk::net {

class HttpUrl {

public:
  class Builder {
  public:

  };

private:
  std::string scheme_;
  std::string user_name_;
  std::string password_;
  std::string host_;
  std::string fragment_;
  std::string url_;
  int port_;
  std::list<std::string> path_segments_;
  std::map<std::string, std::string> query_names_and_values_;
};

}

#endif //NBSDK_HTTP_URL_H
