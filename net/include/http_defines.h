//
// Created by nbaiot@126.com on 2019/12/24.
//
#pragma once

#ifndef NBSDK_HTTP_DEFINES_H
#define NBSDK_HTTP_DEFINES_H

#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/file_body.hpp>
#include <boost/beast/http/buffer_body.hpp>
#include <boost/beast/http/dynamic_body.hpp>

namespace nbsdk::net {

namespace httpBody {
using EmptyBody = boost::beast::http::empty_body;
using StringBody = boost::beast::http::string_body;
using FileBody = boost::beast::http::file_body;
using BufferBody = boost::beast::http::buffer_body;
using DynamicBody = boost::beast::http::dynamic_body;
using VBody = boost::beast::http::dynamic_body;
}

enum HttpProtocol {
  kHTTP_1_0,
  kHTTP_1_1,
  kHTTP_2_0,
};

enum HttpMediaType {

};

}

#endif //NBSDK_HTTP_DEFINES_H
