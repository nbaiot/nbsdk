//
// Created by nbaiot@126.com on 2019/12/23.
//

#pragma once

#ifndef NBSDK_NET_MESSENGER_FACTORY_H
#define NBSDK_NET_MESSENGER_FACTORY_H

#include <string>
#include <memory>

#include <base/include/smart_pointer.h>

#include "messenger_interface.h"

namespace nbsdk::net {

class MessengerFactory {
public:
  static sp<MessengerInterface> CreateWebSocketMessenger(const std::string& url, int port, int threadCount);
};

}

#endif // NBSDK_NET_MESSENGER_FACTORY_H

