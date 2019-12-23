//
// Created by nbaiot@126.com on 2019/12/23.
//

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>

#include "websocket_session.h"

namespace nbsdk::net {

WebsocketSession::WebsocketSession(boost::asio::io_context &ioc, std::string url, int port)
    : host_(std::move(url)), port_(port), state_(kDisconnected),
      resolver_(ioc), ws_(ioc), strand_(ioc.get_executor()) {
}

WebsocketSession::~WebsocketSession() {
  Disconnect();
}

void WebsocketSession::Connect() {
  if (state_ == kDisconnected || state_ == kError) {
    UpdateState(kConnecting);
    resolver_.async_resolve(host_, std::to_string(port_),
                            boost::asio::bind_executor(strand_,
                                                       std::bind(&WebsocketSession::OnResolve, shared_from_this(),
                                                                 std::placeholders::_1, std::placeholders::_2)));
  } else {
    /// LOG_WARNING() << "WebsocketSession current state:" << StateToString(state_) << ", so cancel connect operation";
  }
}


void WebsocketSession::Disconnect() {
  if (state_ != kDisconnected) {
    ws_.async_close(boost::beast::websocket::close_code::normal,
                    boost::asio::bind_executor(strand_,
                        std::bind(&WebsocketSession::OnClosed, shared_from_this(), std::placeholders::_1)));
  }
}

void WebsocketSession::SendData(std::string data) {
  if (state_ == kDisconnected || state_ == kError) {
    /// LOG_ERROR() << "WebsocketSession::SendData failed, current state:" << StateToString(state_);
    return;
  }

  auto ss = std::make_shared<std::string const>(std::move(data));

  boost::asio::post(strand_, std::bind(&WebsocketSession::SendDataInner, shared_from_this(), ss));

}

void
WebsocketSession::OnResolve(boost::system::error_code ec, const boost::asio::ip::tcp::resolver::results_type &results) {
  if (ec) {
    /// LOG_ERROR() << "WebsocketSession::OnResolve failed:" << ec.message();
    UpdateState(kError);
    if (error_callback_) {
      error_callback_(shared_from_this(), kConnectError, ec.message());
    }
    return;
  }
  boost::asio::async_connect(ws_.next_layer(), results.begin(), results.end(),
                             boost::asio::bind_executor(strand_,
                                 std::bind(&WebsocketSession::OnConnect, shared_from_this(), std::placeholders::_1)));
}

void WebsocketSession::OnConnect(boost::system::error_code ec) {
  if (ec) {
    /// LOG_ERROR() << "WebsocketSession::OnConnect failed:" << ec.message();
    UpdateState(kError);
    if (error_callback_) {
      error_callback_(shared_from_this(), kConnectError, ec.message());
    }
    return;
  }
  ws_.async_handshake(host_, "/",
                      boost::asio::bind_executor(strand_,
                          std::bind(&WebsocketSession::OnHandshake, shared_from_this(), std::placeholders::_1)));
}

void WebsocketSession::OnHandshake(boost::system::error_code ec) {
  if (ec) {
    /// LOG_ERROR() << "WebsocketSession::OnHandshake failed:" << ec.message();
    UpdateState(kError);
    if (error_callback_) {
      error_callback_(shared_from_this(), kConnectError, ec.message());
    }
    return;
  }

  UpdateState(kConnected);

  if (connect_callback_) {
    connect_callback_(shared_from_this());
  }

  /// 开始读数据
  DoRead();
}

void WebsocketSession::OnClosed(boost::system::error_code ec) {
  if (ec) {
    /// LOG_ERROR() << "WebsocketSession::OnClosed failed:" << ec.message();
    UpdateState(kError);

    if (error_callback_) {
      error_callback_(shared_from_this(), kCloseError, ec.message());
    }
  } else {
    UpdateState(kDisconnected);
    if (close_callback_) {
      close_callback_(shared_from_this());
    }
  }
}

void WebsocketSession::OnDisconnected(boost::system::error_code ec) {
  /// LOG_ERROR() << "WebsocketSession::OnDisconnected:" << ec.message();
  UpdateState(kError);
  if (error_callback_) {
    error_callback_(shared_from_this(), kDisconnectError, ec.message());
  }
}

void WebsocketSession::OnWrite(boost::system::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  if (ec) {
    /// LOG_ERROR() << "WebsocketSession::OnWrite failed:" << ec.message();
    UpdateState(kError);

    if (error_callback_) {
      error_callback_(shared_from_this(), kSendError, ec.message());
    }
    return;
  }

  send_msg_queue_.erase(send_msg_queue_.begin());

  if (!send_msg_queue_.empty()) {
    ws_.async_write(boost::asio::buffer(*send_msg_queue_.front()),
                    boost::asio::bind_executor(strand_,
                                               std::bind(&WebsocketSession::OnWrite, shared_from_this(),
                                                         std::placeholders::_1, std::placeholders::_2)));
  }
}

void WebsocketSession::OnRead(boost::system::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  /// 本地关闭连接
  /// TODO: fixme 本地断网?
  if (ec == boost::system::errc::operation_canceled) {
    ///LOG_INFO() << "WebsocketSession read canceled";
    return;
  }

  /// 连接被远端关闭
  if (ec == boost::beast::websocket::error::closed) {
    OnDisconnected(ec);
    return;
  }

  /// 读数据出现错误
  if (ec) {
    UpdateState(kError);
    if (error_callback_) {
      error_callback_(shared_from_this(), kReceiveError, ec.message());
    }
    return;
  }

  /// 获取数据
  if (receive_msg_callback_) {
    receive_msg_callback_(shared_from_this(), std::move(boost::beast::buffers_to_string(buffer_.data())));
  }
  /// 清空 buffer
  buffer_.consume(buffer_.size());
  /// 继续读
  DoRead();
}

void WebsocketSession::UpdateState(WebsocketSession::SessionState state) {
  state_ = state;
}

void WebsocketSession::SendDataInner(const std::shared_ptr<const std::string> &data) {

  send_msg_queue_.push_back(data);
  if (send_msg_queue_.size() > 1)
    return;
  ws_.async_write(boost::asio::buffer(*send_msg_queue_.front()),
                  boost::asio::bind_executor(strand_,
                                             std::bind(&WebsocketSession::OnWrite, shared_from_this(),
                                                       std::placeholders::_1, std::placeholders::_2)));
}

void WebsocketSession::DoRead() {

  ws_.async_read(
      buffer_,
      boost::asio::bind_executor(
          strand_,
          std::bind(
              &WebsocketSession::OnRead,
              shared_from_this(),
              std::placeholders::_1,
              std::placeholders::_2)));
}



std::string WebsocketSession::StateToString(WebsocketSession::SessionState state) {
  switch (state) {
    case kUnknown:
      return "Unknown";
    case kDisconnected:
      return "Disconnected";
    case kConnecting:
      return "Connecting";
    case kConnected:
      return "Connected";
    case kError:
      return "NetError";
  }
}

}