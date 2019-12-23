//
// Created by nbaiot@126.com on 2019/12/23.
//

#include <string>

#include "websocket_messenger.h"

namespace nbsdk::net {

WebsocketMessenger::WebsocketMessenger(const std::string &url, int port, int threadCount)
: state_(kMessengerClosed), try_to_recovery_(true) {
  loop_ = std::make_shared<base::EventLoop>(threadCount);
  websocket_ = std::make_shared<WebsocketSession>(loop_->IOContext(), url, port);

  websocket_->SetConnectCallback(std::bind(&WebsocketMessenger::OnConnectedCallback, this, std::placeholders::_1));
  websocket_->SetCloseCallback(std::bind(&WebsocketMessenger::OnClosedCallback, this, std::placeholders::_1));
  websocket_->SetErrorCallback(std::bind(&WebsocketMessenger::OnErrorCallback, this,
      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  websocket_->SetReceiveMsgCallback(std::bind(&WebsocketMessenger::OnReceiveMsgCallback, this,
      std::placeholders::_1, std::placeholders::_2));

  recovery_timer_ = std::make_shared<boost::asio::deadline_timer>(loop_->IOContext());

  loop_->Loop();
}

WebsocketMessenger::~WebsocketMessenger() {
  Close();
  loop_->Exit();
}

void WebsocketMessenger::Open() {
  /// 开启重连机制
  try_to_recovery_ = true;
  if (websocket_) {
    websocket_->Connect();
  }
}

void WebsocketMessenger::Close() {
  /// 停止错误恢复机制
  try_to_recovery_ = false;
  recovery_timer_->cancel();

  if (websocket_) {
    websocket_->Disconnect();
  }
}

void WebsocketMessenger::SendMessage(std::string msg) {
  if (websocket_) {
    websocket_->SendData(std::move(msg));
  }
}

void WebsocketMessenger::SetMessengerListener(const std::weak_ptr<MessengerListener>& listener) {
  listener_ = listener;
}

enum MessengerInterface::MessengerType WebsocketMessenger::MessengerType() {
  return kWebsocket;
}

MessengerInterface::MessengerState WebsocketMessenger::CurrentState() {
  return state_;
}

void WebsocketMessenger::OnConnectedCallback(const std::shared_ptr<WebsocketSession> &ws) {
  MessengerState ori = state_;

  state_ = kMessengerOpened;
  auto listener = listener_.lock();
  if (listener) {
    listener->OnOpened(shared_from_this());
  }
}

void WebsocketMessenger::OnClosedCallback(const std::shared_ptr<WebsocketSession> &ws) {
  MessengerState ori = state_;
  state_ = kMessengerClosed;
  auto listener = listener_.lock();
  if (listener) {
    listener->OnClosed(shared_from_this());
  }
}

void
WebsocketMessenger::OnErrorCallback(const std::shared_ptr<WebsocketSession> &ws, int error, const std::string &reason) {
  MessengerState ori = state_;
  state_ = kMessengerError;

  bool needRestore = false;

  auto listener = listener_.lock();
  switch (error) {
    case WebsocketSession::SessionError::kConnectError:
      needRestore = true;
      if (listener) {
        listener->OnMessengerError(shared_from_this(), MessengerErrorCode::kOpenError, reason);
      }
      break;
    case WebsocketSession::SessionError::kSendError:
      if (listener) {
        listener->OnMessengerError(shared_from_this(), MessengerErrorCode::kSendError, reason);
      }
      break;
    case WebsocketSession::SessionError::kReceiveError:
      needRestore = true;
      if (listener) {
        listener->OnMessengerError(shared_from_this(), MessengerErrorCode::kReceiveError, reason);
      }
      break;
    case WebsocketSession::SessionError::kCloseError:
      break;
    case WebsocketSession::SessionError::kDisconnectError:
      needRestore = true;
      break;
  }

  if (needRestore && try_to_recovery_) {
    recovery_timer_->expires_from_now(boost::posix_time::seconds(5));
    recovery_timer_->async_wait([this](const boost::system::error_code& error){
        if (this->state_ != kMessengerClosed) {
          this->websocket_->Connect();
        }
    });
  }
}

void WebsocketMessenger::OnReceiveMsgCallback(const std::shared_ptr<WebsocketSession> &ws, std::string msg) {
  auto listener = listener_.lock();
  if (listener) {
    listener->OnMessage(shared_from_this(), std::move(msg));
  }
}

}