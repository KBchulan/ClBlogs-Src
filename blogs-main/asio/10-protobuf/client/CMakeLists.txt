cmake_minimum_required(VERSION 3.31)

project(client)

set(CMAKE_CXX_STANDARD 20)
set(CMKAE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PROTOBUF REQUIRED protobuf)

aux_source_directory(${PROJECT_SOURCE_DIR} SRC_LIST)
include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARIES} ws2_32)