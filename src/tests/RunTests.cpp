#include <string>
#include "gtest/gtest.h"

// Enter the test name to run a specific test. 
int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  if (argc > 1) {
  	std::string testName = argv[1];
  	testing::GTEST_FLAG(filter) = testName.append("*");
  }
  return RUN_ALL_TESTS();
}
