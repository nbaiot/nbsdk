//
// Created by nbaiot@126.com on 2019/12/23.
//

#pragma once

#include <memory>
#include <atomic>

#include <boost/asio/io_context.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <messenger_interface.h>

#include <base/include/event_loop.h>

#include "websocket_session.h"

namespace nbsdk::net {

class WebsocketMessenger : public MessengerInterface, public std::enable_shared_from_this<WebsocketMessenger> {
public:

  explicit WebsocketMessenger(const std::string& url, int port, int threadCount = 1);

  ~WebsocketMessenger();

  void Open() override;

  void Close() override;

  void SendMessage(std::string msg) override;

  void SetMessengerListener(const std::weak_ptr<MessengerListener>& listener) override;

  enum MessengerType MessengerType() override;

  MessengerState CurrentState() override;


private:
  /// session callback
  void OnConnectedCallback(const std::shared_ptr<WebsocketSession> &ws);

  void OnClosedCallback(const std::shared_ptr<WebsocketSession> &ws);

  void OnErrorCallback(const std::shared_ptr<WebsocketSession> &ws,
                       int error, const std::string &reason);

  void OnReceiveMsgCallback(const std::shared_ptr<WebsocketSession> &ws, std::string msg);

  DISALLOW_COPY_AND_ASSIGN(WebsocketMessenger);
private:
  std::shared_ptr<WebsocketSession> websocket_;
  std::shared_ptr<base::EventLoop> loop_;
  std::atomic<MessengerState> state_;
  std::atomic_bool try_to_recovery_;
  std::weak_ptr<MessengerListener> listener_;
  std::shared_ptr<boost::asio::deadline_timer> recovery_timer_;
};

}


