cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0016 NEW)

project(rabbitServerTests VERSION 1.0.0 LANGUAGES CXX DESCRIPTION "Тесты")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Protobuf REQUIRED)
find_package(rabbitmq-c REQUIRED)
find_package(GTest REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src/server)
include_directories(${CMAKE_SOURCE_DIR}/src/server/include)
include_directories(${CMAKE_SOURCE_DIR}/test)
include_directories(${GTEST_INCLUDE_DIRS})


protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/src/protocol/Messages.proto)

# Tests
add_executable(TestRabbitServer
    ${PROTO_SRCS} ${PROTO_HDRS}
  ${CMAKE_SOURCE_DIR}/src/server/include/utils.h ${CMAKE_SOURCE_DIR}/src/server/utils.cpp
  ${CMAKE_SOURCE_DIR}/src/server/include/logger.h ${CMAKE_SOURCE_DIR}/src/server/logger.cpp
  ${CMAKE_SOURCE_DIR}/src/server/include/rabbitserver.h ${CMAKE_SOURCE_DIR}/src/server/rabbitserver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/testServer.cpp
)

target_link_libraries(TestRabbitServer GTest::GTest GTest::Main rabbitmq::rabbitmq ${Protobuf_LIBRARIES} Qt5::Core)

add_test(NAME TestRabbitServer COMMAND TestRabbitServer)
