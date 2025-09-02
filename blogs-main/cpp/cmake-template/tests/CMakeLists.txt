# 测试文件
file(GLOB_RECURSE TEST_SOURCES "*.cc")

add_executable(run_tests ${TEST_SOURCES})

# 链接 GTest 库
target_link_libraries(run_tests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(run_tests)