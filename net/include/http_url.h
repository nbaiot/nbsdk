//
// Created by nbaiot@126.com on 2019/12/24.
//
#pragma once

#ifndef NBSDK_HTTP_URL_H
#define NBSDK_HTTP_URL_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>

namespace nbsdk::net {
/**
    http://example.com:8888/over/there?name=ferret#nose
    \__/ \______________/ \________/\_________/ \__/

    |         |              |         |        |

 scheme     host           path      query   fragment
 */

/**
 * https://www.cnblogs.com/liuhongfeng/p/5006341.html
 */
class HttpUrl {

public:
  class Builder {
  public:
    Builder();

    Builder& Scheme(const std::string& scheme);

    /**
     *
     * @param host
     * either a regular hostname, International Domain Name, IPv4 address
     * TODO: add IPv6 address
     * @return
     */
    Builder& Host(const std::string& host);

    Builder& Port(int port);

    /**
     * 单一 segment
     * @param pathSegment
     * @return
     */
    Builder& AddEncodedPathSegment(const std::string& pathSegment);

    /// TODO: fixme
    ///Builder& AddPathSegment(const std::string& pathSegment);

    /**
     * 组合 segment
     * eg:/a/b/c/ or a/b/c
     * @param pathSegment
     * @return
     */
    Builder& AddEncodedPathSegments(const std::string& pathSegment);
    /// TODO: fixme
    ///Builder& AddPathSegments(const std::string& pathSegment);

    Builder& SetEncodedPathSegment(int index, const std::string& pathSegment);

    /// TODO: fixme
    ///Builder& SetPathSegment(int index, const std::string& pathSegment);

    Builder& RemovePathSegment(int index);

    Builder& AddEncodedQueryParameter(const std::string& name, const std::string& value);

    Builder& RemoveEncodedQueryParameter(const std::string& name, const std::string& value);

    Builder& RemoveEncodedQueryParameter(const std::string& name);

    Builder& AddEncodedFragment(const std::string& fragment);

    HttpUrl Build();

    bool Parse(std::string url);


  private:
    void Pop();

    void Push(const std::string& segment);

    void Clear() {
      scheme_.clear();
      host_.clear();
      port_ = -1;
      encoded_path_segments_.clear();
      encoded_query_names_and_values_.clear();
      encoded_fragment_.clear();
    }
  private:
    friend class HttpUrl;
    std::string scheme_;
    std::string host_;
    int port_{-1};
    std::list<std::string> encoded_path_segments_;
    /// TODO: fixme, name 可能重复， value 可能空
    std::map<std::string, std::set<std::string>> encoded_query_names_and_values_;
    std::string encoded_fragment_;

  };

public:

  explicit HttpUrl(const Builder& builder);

  explicit HttpUrl(std::string  url);

  std::string Scheme();

  int Port();

  bool IsHttps();

  std::string Host();

  int PathSize();

  std::string EncodedPath();

  std::string EncodedQuery();

  int QuerySize();

  std::set<std::string> QueryParameter();

  std::string EncodedFragment();

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
  std::map<std::string, std::set<std::string>> query_names_and_values_;

  /** Canonical URL. */
  std::string url_;


};

}

#endif //NBSDK_HTTP_URL_H
