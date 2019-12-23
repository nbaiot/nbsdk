//
// Created by nbaiot@126.com on 2019/12/23.
//

#pragma once

#include <memory>
#include <functional>
#include <atomic>
#include <vector>
#include <mutex>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>

#include <base/include/macros.h>

namespace nbsdk::net {

class WebsocketSession : public std::enable_shared_from_this<WebsocketSession> {

public:

  enum SessionError {
    kUnknownError = 0,
    kConnectError,
    kSendError,
    kReceiveError,
    kCloseError,
    kDisconnectError,
  };

  typedef std::function<void(const std::shared_ptr<WebsocketSession> &ws)> ConnectCallback;
  typedef std::function<void(const std::shared_ptr<WebsocketSession> &ws)> CloseCallback;
  typedef std::function<void(const std::shared_ptr<WebsocketSession> &ws,
                             int error, const std::string &reason)> ErrorCallback;
  typedef std::function<void(const std::shared_ptr<WebsocketSession> &ws, std::string msg)> ReceiveMsgCallback;

  enum SessionState {
    kUnknown = 0,
    kDisconnected,
    kConnecting,
    kConnected,
    kError,
  };

  WebsocketSession(boost::asio::io_context& ioc_, std::string url, int port);

  ~WebsocketSession();

  void Connect();

  void Disconnect();

  /**
   * 在 io_context 线程中发送数据
   * @param data
   */
  void SendData(std::string data);

  void SetConnectCallback(const ConnectCallback& callback) {
    connect_callback_ = callback;
  }

  void SetCloseCallback(const CloseCallback& callback) {
    close_callback_ = callback;
  }

  void SetErrorCallback(const ErrorCallback& callback) {
    error_callback_ = callback;
  }

  void SetReceiveMsgCallback(const ReceiveMsgCallback& callback) {
    receive_msg_callback_ = callback;
  }

  SessionState CurrentState() const {
    return state_;
  }

  std::string HostUrl() const {
    return host_;
  }

  int HostPort() const {
    return port_;
  }

private:
  void OnResolve(boost::system::error_code ec,
                 const boost::asio::ip::tcp::resolver::results_type& results);

  void OnConnect(boost::system::error_code ec);

  void OnHandshake(boost::system::error_code ec);

  void OnDisconnected(boost::system::error_code ec);

  void OnClosed(boost::system::error_code ec);

  void OnWrite(boost::system::error_code ec, std::size_t bytes_transferred);

  void OnRead(boost::system::error_code ec, std::size_t bytes_transferred);

  static std::string StateToString(SessionState state);

  void UpdateState(SessionState state);

  void SendDataInner(const std::shared_ptr<std::string const> & data);

  void DoRead();

  DISALLOW_COPY_AND_ASSIGN(WebsocketSession);
private:
  std::string host_;
  int port_;
  std::atomic<SessionState> state_;
  boost::asio::ip::tcp::resolver resolver_;
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws_;
  boost::asio::strand<boost::asio::io_context::executor_type> strand_;
  boost::beast::multi_buffer buffer_;
  std::vector<std::shared_ptr<std::string const>> send_msg_queue_;
  /// callback
  ConnectCallback connect_callback_;
  CloseCallback close_callback_;
  ErrorCallback  error_callback_;
  ReceiveMsgCallback receive_msg_callback_;
};

}

