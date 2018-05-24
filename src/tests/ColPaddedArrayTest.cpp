#include "../../includes/ColPaddedArray.h"
#include "../../includes/globals.h"

#include "gtest/gtest.h"
#include <iostream>
#include <vector>

TEST(ColPaddedArrayTest, getArray) {
	std::vector<std::vector<int32_t>> m = {{1, 2, 3, 1, 3}, {1, 2}, {1, 2, 3}};
	std::unique_ptr<ColPaddedArray> colPaddedArray(new ColPaddedArray("A", 32, false, m));
	std::vector<int32_t> expected = {1, 1, 1, 2, 2, 2, 3, 0, 3, 1, 0, 0, 3, 0, 0};

	EXPECT_EQ(colPaddedArray->getArray(), expected);
	EXPECT_EQ(colPaddedArray->elementAt(0, 2), 3);
	EXPECT_EQ(colPaddedArray->elementAt(1, 0), 1);
	EXPECT_EQ(colPaddedArray->elementAt(2, 4), 0);
}
