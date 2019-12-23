//
// Created by nbaiot@126.com on 2019/12/23.
//

#include <messenger_factory.h>

#include "websocket_messenger.h"

namespace nbsdk::net {

sp<MessengerInterface> MessengerFactory::CreateWebSocketMessenger(const std::string &url, int port, int threadCount) {
  return std::make_shared<WebsocketMessenger>(url, port, threadCount);
}

}