#include "../../includes/ColPaddedArray.h"
#include "../../includes/globals.h"

#include "gtest/gtest.h"
#include <iostream>
#include <vector>

TEST(ColPaddedArrayTest, getArray) {
	std::vector<std::vector<int32_t>> m = {{1, 2, 3, 1, 3}, {1, 2}, {1, 2, 3}};
	std::unique_ptr<ColPaddedArray> colPaddedArray(new ColPaddedArray("A", 32, false, m));
	std::vector<int32_t> expected;

	for (int i = 0; i < m.size(); i++) {
    for (int j = 0; j < 5; j++) {
      if (j >= m.at(i).size()) {
        m.at(i).push_back(0);
      }
    }
  }

  for (int j = 0; j < 5; j++) {
  	for (int i = 0; i < m.size(); i++) {
  		expected.push_back(m.at(i).at(j));
  	}
  }

	EXPECT_EQ(colPaddedArray->getArray(), expected);
	EXPECT_EQ(colPaddedArray->elementAt(0, 2), 3);
	EXPECT_EQ(colPaddedArray->elementAt(1, 0), 1);
	EXPECT_EQ(colPaddedArray->elementAt(2, 4), 0);
}
