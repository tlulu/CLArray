#include "../../includes/RowPaddedArray.h"
#include "../../includes/globals.h"

#include "gtest/gtest.h"
#include <iostream>
#include <vector>

TEST(RowPaddedArrayTest, getArray) {
	std::vector<std::vector<int32_t>> m = {{1, 2, 3, 1, 3}, {1, 2}, {1, 2, 3}};
	std::unique_ptr<RowPaddedArray> rowPaddedArray(new RowPaddedArray("A", 32, false, m));
	std::vector<int32_t> expected = {1, 2, 3, 1, 3, 1, 2, 0, 0, 0, 1, 2, 3, 0, 0};

	EXPECT_EQ(rowPaddedArray->getArray(), expected);
	EXPECT_EQ(rowPaddedArray->elementAt(0, 2), 3);
	EXPECT_EQ(rowPaddedArray->elementAt(1, 0), 1);
	EXPECT_EQ(rowPaddedArray->elementAt(2, 4), 0);
}
