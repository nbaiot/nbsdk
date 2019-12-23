//
// Created by nbaiot@126.com on 2019/12/23.
//

#pragma once

#ifndef NBSDK_NET_MESSENGER_INTERFACE_H
#define NBSDK_NET_MESSENGER_INTERFACE_H

#include <memory>

#include <base/include/smart_pointer.h>

namespace nbsdk::net {

class MessengerListener;

class MessengerInterface {

public:

  enum MessengerType {
    kUnknownType = 0,
    kWebsocket,
  };

  enum MessengerErrorCode {
    kNoError = 0,
    kOpenError,
    kSendError,
    kReceiveError,
  };

  enum MessengerState {
    kUnknownState = 0,
    kMessengerClosed,
    kMessengerOpened,
    kMessengerError,
  };

  virtual void Open() = 0;

  virtual void Close() = 0;

  virtual void SendMessage(std::string msg) = 0;

  virtual void SetMessengerListener(const wp<MessengerListener> &listener) = 0;

  virtual enum MessengerState CurrentState() = 0;

  virtual enum MessengerType MessengerType() = 0;

  static std::string StateToString(MessengerState state) {
    switch (state) {
      case kUnknownState:
        return "MessengerState";
      case kMessengerClosed:
        return "MessengerClosed";
      case kMessengerOpened:
        return "MessengerOpened";
      case kMessengerError:
        return "MessengerError";
    }
  }

  static std::string ErrorToString(MessengerErrorCode code) {
    switch (code) {
      case kNoError:
        return "NoError";
      case kOpenError:
        return "OpenError";
      case kSendError:
        return "SendError";
      case kReceiveError:
        return "ReceiveError";
    }
  }

};

class MessengerListener {
public:
  virtual void OnOpened(const sp<MessengerInterface> &messenger) = 0;

  virtual void OnClosed(const sp<MessengerInterface> &messenger) = 0;

  virtual void OnMessage(const sp<MessengerInterface> &messenger, std::string msg) = 0;

  virtual void OnMessengerError(const sp<MessengerInterface> &messenger,
                                MessengerInterface::MessengerErrorCode code, const std::string &reason) = 0;
};

}

#endif // NBSDK_NET_MESSENGER_INTERFACE_H

