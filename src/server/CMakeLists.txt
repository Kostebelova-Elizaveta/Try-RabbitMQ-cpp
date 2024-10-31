cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0016 NEW)

project(rabbitServer VERSION 1.0.0 LANGUAGES CXX DESCRIPTION "Сервер")

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
include_directories(${CMAKE_SOURCE_DIR}/src/server/config)
include_directories(${GTEST_INCLUDE_DIRS})

set(SERVER_CONFIG_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config/config.ini")
add_definitions(-DSERVER_CONFIG_FILE_PATH="${SERVER_CONFIG_FILE_PATH}")

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/src/protocol/Messages.proto)

add_executable(rabbitServer
    ${PROTO_SRCS} ${PROTO_HDRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/utils.h ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/logger.h ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/rabbitserver.h ${CMAKE_CURRENT_SOURCE_DIR}/rabbitserver.cpp
)

target_link_libraries(rabbitServer PRIVATE rabbitmq::rabbitmq ${Protobuf_LIBRARIES} Qt5::Core)
