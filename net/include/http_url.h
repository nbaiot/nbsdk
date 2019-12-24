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

namespace httpUrl {

class Builder {
public:
  Builder() = default;

  Builder& Scheme(const std::string& scheme);

  /**
   *
   * @param host
   * either a regular hostname, International Domain Name, IPv4 address, or IPv6 address
   * @return
   */
  Builder& Host(const std::string& host);

  Builder& Port(int port);

private:
  std::string scheme_;
  std::string host_;
  int port_{-1};
  std::list<std::string> encoded_path_segments_;
  /// TODO: fixme, name 可能重复， value 可能空
  std::map<std::string, std::string> encoded_query_names_and_values_;
  std::string encoded_fragment_;


};
}

/**
 * https://www.cnblogs.com/tengjiang/p/10344225.html
 */
class HttpUrl {

public:
  static int DefaultPort(const std::string& scheme);
private:
  /** http or https */
  std::string scheme_;
  /** Either 80, 443 or a user-specified port. In range [1..65535]. */
  int port_;
  /** Canonical hostname */
  std::string host_;
  /** Decoded fragment. */
  std::string fragment_;

  /**
   * A list of canonical path segments. This list always contains at least one element, which may be
   * the empty string. Each segment is formatted with a leading '/', so if path segments were ["a",
   * "b", ""], then the encoded path would be "/a/b/".
   */
  std::list<std::string> path_segments_;
  /**
   * Alternating, decoded query names and values, or null for no query. Names may be empty or
   * non-empty, but never null. Values are null if the name has no corresponding '=' separator, or
   * empty, or non-empty.
   */
  std::map<std::string, std::string> query_names_and_values_;

  /** Canonical URL. */
  std::string url_;
};

}

#endif //NBSDK_HTTP_URL_H
