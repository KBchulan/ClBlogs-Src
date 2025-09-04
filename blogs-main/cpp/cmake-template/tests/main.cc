#include <gtest/gtest.h>

TEST(HelloTest, BasicAssertions)
{
  EXPECT_STREQ("hello", "hello");
  EXPECT_EQ(7 * 6, 42);
}
