//
// Created by nbaiot@126.com on 2019/12/5.
//

#pragma once

#ifndef NBSDK_MACROS_H
#define NBSDK_MACROS_H

namespace nbsdk {

#define DISALLOW_COPY(TypeName)  TypeName(const TypeName&) = delete

#define DISALLOW_ASSIGN(TypeName) TypeName& operator=(const TypeName&) = delete

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  DISALLOW_COPY(TypeName);                 \
  DISALLOW_ASSIGN(TypeName)

}

#endif //NBSDK_MACROS_H
  