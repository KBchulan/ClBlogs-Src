# Logger单元测试
add_executable(test_logger middleware/test_logger.cc)

target_include_directories(test_logger
  PRIVATE ${PROJECT_SOURCE_DIR}/include
)


######## 测试文件写在上面 ########

target_link_libraries(test_logger
  PRIVATE
  GTest::gtest_main
  fmt::fmt
)

target_compile_features(test_logger PRIVATE cxx_std_23)

set_warning_flags(test_logger)

if(ENABLE_ASAN)
  target_compile_options(test_logger PRIVATE -fsanitize=address -fno-omit-frame-pointer)
  target_link_options(test_logger PRIVATE -fsanitize=address)
endif()

if(ENABLE_UBSAN)
  target_compile_options(test_logger PRIVATE -fsanitize=undefined)
  target_link_options(test_logger PRIVATE -fsanitize=undefined)
endif()

include(GoogleTest)
gtest_discover_tests(test_logger)
