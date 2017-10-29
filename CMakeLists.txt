cmake_minimum_required(VERSION 3.5)
project(protobuf_trie)

## Protobuf
include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER trie.proto)
add_library(proto ${PROTO_HEADER} ${PROTO_SRC})


## CPP
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -Wall -O3")
set(SOURCE_FILES main.cpp)

add_executable(protobuf_trie ${SOURCE_FILES} ${PROTOBUF_LIBRARY})
target_link_libraries(protobuf_trie proto ${PROTOBUF_LIBRARY})