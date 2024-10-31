cmake_minimum_required(VERSION 3.15.0)
cmake_policy(SET CMP0016 NEW)

project(rabbitClient VERSION 1.0.0 LANGUAGES CXX DESCRIPTION "Клиент")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Protobuf REQUIRED)
find_package(rabbitmq-c REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src/client)
include_directories(${CMAKE_SOURCE_DIR}/src/client/include)
include_directories(${CMAKE_SOURCE_DIR}/src/client/config)

set(CLIENT_CONFIG_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config/config.ini")
add_definitions(-DCLIENT_CONFIG_FILE_PATH="${CLIENT_CONFIG_FILE_PATH}")

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/src/protocol/Messages.proto)

set(PROJECT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/mainwindow.h
        ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/include/utils.h ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/logger.h ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dialogtoeditsettings.h ${CMAKE_CURRENT_SOURCE_DIR}/dialogtoeditsettings.cpp ${CMAKE_CURRENT_SOURCE_DIR}/dialogtoeditsettings.ui
        ${CMAKE_CURRENT_SOURCE_DIR}/include/queuelistener.h ${CMAKE_CURRENT_SOURCE_DIR}/queuelistener.cpp
)

add_executable(rabbitClient
    ${PROTO_SRCS} ${PROTO_HDRS}
            ${PROJECT_SOURCES}
        )

target_link_libraries(rabbitClient PRIVATE rabbitmq::rabbitmq ${Protobuf_LIBRARIES} Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(rabbitClient PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


