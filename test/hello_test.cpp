#include <gtest/gtest.h>

TEST(HelloTest, HelloWorld) {
  // Arrange
  std::string expected = "Hello, World!";

  // Act
  std::string actual = "Hello, World!";

  // Assert
  EXPECT_EQ(expected, actual);
}