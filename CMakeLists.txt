cmake_minimum_required(VERSION 3.15)
project(nbsdk)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_LIST_DIR})

### compiler version
message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}, version: "
        "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}, version: "
        "${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")

### for platform
if(ANDROID)
    add_definitions(-DOS_ANDROID)
elseif(UNIX)
    add_definitions(-DOS_UNIX)
endif()

### for boost
set(Boost_NO_BOOST_CMAKE TRUE)
set(BOOST_ROOT /opt/third_party/boost/boost_1.71)

find_package(Boost REQUIRED COMPONENTS system thread)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    MESSAGE( STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}")
    MESSAGE( STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")
    MESSAGE( STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}")
endif()

### include module
add_subdirectory(base)

add_subdirectory(test)



