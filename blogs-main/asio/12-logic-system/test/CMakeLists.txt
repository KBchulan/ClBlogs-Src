cmake_minimum_required(VERSION 3.31)

project(jsonCppTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

add_executable(${PROJECT_NAME} read.cc)
target_link_libraries(${PROJECT_NAME} PRIVATE ${JSONCPP_LIBRARIES})