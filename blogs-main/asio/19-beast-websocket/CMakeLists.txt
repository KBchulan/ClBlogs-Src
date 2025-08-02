cmake_minimum_required(VERSION 3.25)

project(
  WebSocketServer
  VERSION 1.0.0
)

# DEBUG模式给一个'd'后缀
set(CMAKE_DEBUG_POSTFIX d)

# 设置默认构建类型
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# 生成编译命令
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 代码质量检测函数
function(set_warning_flags target)
  get_target_property(target_type ${target} TYPE)

  if(target_type STREQUAL "INTERFACE_LIBRARY")
    set(visibility INTERFACE)
  else()
    set(visibility PRIVATE)
  endif()

  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${target} ${visibility}
      -Wall
      -Wextra
      -Wpedantic
    )
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${target} ${visibility}
      -Wall
      -Wextra
      -Wpedantic
    )
  endif()
endfunction()

# 配置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找Boost库
find_package(Boost REQUIRED COMPONENTS system)

# 主程序
add_executable(${PROJECT_NAME} server.cc)
set_warning_flags(${PROJECT_NAME})

# 设置C++23标准
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# Debug版本设置后缀
set_target_properties(
  ${PROJECT_NAME} PROPERTIES
  DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
)

# 第三方库链接
target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  ${Boost_LIBRARIES}
)

# Windows特定链接
if(WIN32)
  target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    ws2_32
    mswsock
    stdc++exp
  )
else()
  target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    pthread
    stdc++exp
  )
endif()

# Boost头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})

# Header-only宏定义
target_compile_definitions(${PROJECT_NAME} PRIVATE
  BOOST_ASIO_HEADER_ONLY
  BOOST_BEAST_HEADER_ONLY
  BOOST_UUID_HEADER_ONLY
)