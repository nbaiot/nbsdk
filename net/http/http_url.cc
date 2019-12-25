//
// Created by nbaiot@126.com on 2019/12/24.
//
#include <algorithm>
#include <regex>
#include <utility>

#include <http_url.h>

#include <base/include/nlog.h>

using namespace nbsdk::base;

namespace nbsdk::net {

HttpUrl::Builder::Builder() {
  /// The default path is '/' which needs a trailing space.
  encoded_path_segments_.emplace_back("");
}

HttpUrl::Builder &HttpUrl::Builder::Scheme(const std::string &scheme) {
  if (scheme.empty()) {
    LOG_ERROR() << "scheme empty, use default scheme: http";
    scheme_ = "http";
  }
  if (scheme == "http")
    scheme_ = "http";
  else if (scheme == "https")
    scheme_ = "https";
  else {
    LOG_ERROR() << "unexpected scheme:" << scheme << ", use default scheme: http";
    scheme_ = "http";
  }
  return *this;
}

HttpUrl::Builder &HttpUrl::Builder::Host(const std::string &host) {
  if (host.empty()) {
    LOG_ERROR() << "host empty";
    return *this;
  } else {
    host_.clear();
    std::transform(host.begin(), host.end(), host_.begin(), ::tolower);
  }
  return *this;
}

HttpUrl::Builder &HttpUrl::Builder::Port(int port) {
  if (port <= 0 || port > 65535) {
    LOG_ERROR() << "unexpected port:" << port;
    port_ = -1;
  } else {
    port_ = port;
  }
  return *this;
}

HttpUrl::Builder &HttpUrl::Builder::AddEncodedPathSegment(const std::string &pathSegment) {
  if (pathSegment.empty()) {
    LOG_ERROR() << "pathSegments empty";
    return *this;
  }
  Push(pathSegment);
  return *this;
}

HttpUrl::Builder &HttpUrl::Builder::AddEncodedPathSegments(const std::string &pathSegment) {
  if (pathSegment.empty()) {
    LOG_ERROR() << "pathSegments empty";
    return *this;
  }
  std::regex re{"/"};
  auto ss = std::vector<std::string>{std::sregex_token_iterator(pathSegment.begin(), pathSegment.end(), re, -1),
                                     std::sregex_token_iterator()};
  for (auto &it : ss) {
    if (!it.empty())
      Push(it);
  }
  return *this;
}

/**
 * [""] ==> [""]
 * ["a", "b", "c"] ==> ["a", "b", ""]
 * ["a", "b", "c", ""] ==> ["a", "b", ""]
 */
void HttpUrl::Builder::Pop() {
  auto last = --encoded_path_segments_.end();
  auto it = encoded_path_segments_.erase(last);
  /// 如果 pop 的是 "", 而且 encoded_path_segments_ 不为 empty, 那么继续 pop
  /// eg: ["a", "b", "c", ""] ==> ["a", "b", ""]
  if ((*it).empty() && !encoded_path_segments_.empty()) {
    encoded_path_segments_.pop_back();
  }
  encoded_path_segments_.emplace_back("");
}

void HttpUrl::Builder::Push(const std::string &segment) {
  if (segment.empty())
    return;
  /// TODO: encode segment
  if (segment == ".") {
    return;
  }
  if (segment == "..") {
    Pop();
    return;
  }
  /// 保证 encoded_path_segments_ 最后一个元素始终为 ""
  encoded_path_segments_.emplace(--encoded_path_segments_.end(), segment);
}

HttpUrl::Builder &HttpUrl::Builder::SetEncodedPathSegment(int index, const std::string &pathSegment) {
  if (pathSegment.empty()) {
    LOG_ERROR() << "pathSegments empty";
    return *this;
  }
  if (pathSegment == "." || pathSegment == "..") {
    LOG_ERROR() << "unexpected pathSegment:" << pathSegment;
    return *this;
  }
  if (index < 0 || index >= encoded_path_segments_.size()) {
    LOG_ERROR() << "unexpected index:" << index;
    return *this;
  }
  auto it = encoded_path_segments_.begin();
  for (int i = 0; i < index; ++i) {
    ++it;
  }
  *it = pathSegment;

  /// 保证最后一个元素为 ""
  if (index == (encoded_path_segments_.size() - 1)) {
    encoded_path_segments_.emplace_back("");
  }
  return *this;
}

HttpUrl::Builder &HttpUrl::Builder::RemovePathSegment(int index) {
  if (index < 0 || index >= encoded_path_segments_.size()) {
    LOG_ERROR() << "unexpected index:" << index;
    return *this;
  }
  bool isLast = index == (encoded_path_segments_.size() - 1);
  auto it = encoded_path_segments_.begin();
  for (int i = 0; i < index; ++i) {
    ++it;
  }
  encoded_path_segments_.erase(it);
  /// 保证最后一个元素为 ""
  if (isLast) {
    encoded_path_segments_.emplace_back("");
  }
  return *this;
}

HttpUrl::Builder &HttpUrl::Builder::AddEncodedQueryParameter(const std::string& name, const std::string& value) {
  if (name.empty()) {
    LOG_ERROR() << "unexpected name:" << name;
    return *this;
  }
  auto it = encoded_query_names_and_values_.find(name);
  if (it == encoded_query_names_and_values_.end()) {
    std::set<std::string> values;
    values.emplace(value);
    encoded_query_names_and_values_.emplace(std::make_pair(name, values));
  } else {
    it->second.emplace(value);
  }
  return *this;
}

HttpUrl::Builder &HttpUrl::Builder::RemoveEncodedQueryParameter(const std::string &name, const std::string &value) {
  if (name.empty()) {
    LOG_ERROR() << "unexpected name:" << name;
    return *this;
  }
  auto it = encoded_query_names_and_values_.find(name);
  if (it != encoded_query_names_and_values_.end()) {
    it->second.erase(value);
    if (it->second.empty()) {
      encoded_query_names_and_values_.erase(name);
    }
  }
  return *this;
}

HttpUrl::Builder &HttpUrl::Builder::RemoveEncodedQueryParameter(const std::string &name) {
  if (name.empty()) {
    LOG_ERROR() << "unexpected name:" << name;
    return *this;
  }
  encoded_query_names_and_values_.erase(name);
  return *this;
}

HttpUrl::Builder &HttpUrl::Builder::AddEncodedFragment(const std::string &fragment) {
  if (fragment.empty()) {
    LOG_ERROR() << "unexpected fragment:" << fragment;
    return *this;
  }
  encoded_fragment_ = fragment;
  return *this;
}

HttpUrl HttpUrl::Builder::Build() {
  if (scheme_.empty() || host_.empty()) {
    LOG_ERROR() << "scheme or host empty";
  }
  return HttpUrl(*this);
}

bool HttpUrl::Builder::Parse(std::string url) {
  if (url.empty() || url.size() <= 7) {
    LOG_ERROR() << "unexpected url:" << url;
    return false;
  }

  Clear();

  url.erase(0, url.find_first_not_of(' '));
  url.erase(0, url.find_first_not_of('\t'));
  url.erase(0, url.find_first_not_of('\n'));
  url.erase(0, url.find_first_not_of('\f'));
  url.erase(0, url.find_first_not_of('\r'));

  url.erase(url.find_last_not_of(' '));
  url.erase(url.find_last_not_of('\t'));
  url.erase(url.find_last_not_of('\n'));
  url.erase(url.find_last_not_of('\f'));
  url.erase(url.find_last_not_of('\r'));

  /// scheme
  auto scheme = url.substr(0, url.find_first_of(':'));
  if (scheme.empty()) {
    LOG_ERROR() << "unexpected url:" << url;
    return false;
  }
  std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);
  if (scheme == "http") {
    scheme_ = "http";
  } else if (scheme == "https") {
    scheme_ = "https";
  } else {
    LOG_ERROR() << "unexpected url:" << url;
    return false;
  }

  /// host+path
  /// TODO: fixme, ignore username and password
  auto pathStart = scheme_.size() + 3;/// ://
  auto pathEnd = url.find('?');
  if (pathEnd == std::string::npos) {
    pathEnd = url.find('#');
  }
  if (pathEnd == std::string::npos) {
    pathEnd = url.size();
  }
  auto hostPath =  url.substr(pathStart, pathEnd - pathStart);

  if (hostPath.empty()) {
    LOG_ERROR() << "unexpected url:" << url;
    return false;
  }

  /// host
  auto portStart = hostPath.find_first_of(':');
  pathStart = hostPath.find_first_of('/');

  if (portStart == std::string::npos) {
    /// port
    port_ = DefaultPort(scheme_);
    if (pathStart ==  std::string::npos) {
      host_ = hostPath;
    } else {
      host_ = hostPath.substr(0, pathStart);
    }
  } else {
    host_ = hostPath.substr(0, portStart);
    auto portEnd = hostPath.find_first_of('/');
    std::string portS;
    if (portEnd == std::string::npos) {
      portS = hostPath.substr(portStart + 1);
    } else {
      portS = hostPath.substr(portStart + 1, portEnd - (portStart + 1));
    }
    if (!portS.empty()) {
      /// TODO: fixme, not number?
      port_ = std::atoi(portS.c_str());
    } else {
      port_ = DefaultPort(scheme_);
    }
  }

  /// path
  auto path = hostPath.substr(pathStart + 1);
  if (!path.empty()) {
    /// TODO: fixme, encode path
    AddEncodedPathSegments(path);
  }

  auto queryStart = url.find_first_of('?');
  auto fragmentStart = url.find_first_of('#');
  /// query
  std::string query;
  if (queryStart != std::string::npos) {
    if (fragmentStart == std::string::npos) {
      query = url.substr(queryStart + 1);
    } else {
      query = url.substr(queryStart + 1, fragmentStart - (queryStart + 1));
    }
  }
  /// TODO: fixme, encode query
  if (!query.empty()) {
    std::regex re{"&"};
    auto ss = std::vector<std::string>{std::sregex_token_iterator(query.begin(), query.end(), re, -1),
                                       std::sregex_token_iterator()};
    for (auto &s : ss) {
      if (!s.empty()) {
        std::string param;
        std::string value;
        /// find '='
        auto p = s.find_first_of('=');
        if (p != std::string::npos) {
          param = s.substr(0, p);
          value = s.substr(p + 1);
        } else {
          param = s;
        }
        AddEncodedQueryParameter(param, value);
      }
    }
  }

  /// fragment
  /// TODO: fixme, encode fragment
  if (fragmentStart != std::string::npos) {
    AddEncodedFragment(url.substr(fragmentStart + 1));
  }

  return true;
}


int HttpUrl::DefaultPort(const std::string &scheme) {
  if (scheme == "http")
    return 80;
  else if (scheme == "https")
    return 443;
  return -1;
}

HttpUrl::HttpUrl(std::string url) : url_(std::move(url)) {
scheme_ = "https";
}

HttpUrl::HttpUrl(const HttpUrl::Builder &builder) {
  scheme_ = builder.scheme_;
  host_ = builder.host_;
  port_ = builder.port_;
  path_segments_ = builder.encoded_path_segments_;
  query_names_and_values_ = builder.encoded_query_names_and_values_;
  fragment_ = builder.encoded_fragment_;
}

std::string HttpUrl::Scheme() {
  return scheme_;
}

int HttpUrl::Port() {
  return port_;
}

bool HttpUrl::IsHttps() {
  return scheme_ == "https";
}

std::string HttpUrl::Host() {
  return host_;
}

int HttpUrl::PathSize() {
  /// 始终包含 ""
  return (int)path_segments_.size() - 1;
}

std::string HttpUrl::EncodedPath() {
  auto pathStart = scheme_.size() + 3;/// ://
  auto pathEnd = url_.find('?');
  if (pathEnd == std::string::npos) {
    pathEnd = url_.find('#');
  }
  if (pathEnd == std::string::npos) {
    pathEnd = url_.size();
  }
  return url_.substr(pathStart, pathEnd - pathStart);
}

std::string HttpUrl::EncodedQuery() {

  return "std::__cxx11::string()";
}

int HttpUrl::QuerySize() {
  return 0;
}

std::set<std::string> HttpUrl::QueryParameter() {
  return std::set<std::string>();
}

std::string HttpUrl::EncodedFragment() {
  return fragment_;
}
}