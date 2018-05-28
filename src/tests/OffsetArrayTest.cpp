#include "../../includes/OffsetArray.h"
#include "../../includes/globals.h"

#include "gtest/gtest.h"
#include <iostream>
#include <vector>

TEST(OffsetArrayTest, getArray) {
	std::vector<std::vector<int32_t>> m = {{1, 2, 3, 1, 3}, {1, 2}, {1, 2, 3}};
	std::unique_ptr<OffsetArray> offsetArray(new OffsetArray("A", 32, false, m));
	std::vector<int32_t> expectedValues = {1, 2, 3, 1, 3, 1, 2, 1, 2, 3};
	std::vector<int32_t> expectedOffsets = {0, 5, 7, 10};

	EXPECT_EQ(offsetArray->getArray(), expectedValues);
	EXPECT_EQ(offsetArray->getOffsets(), expectedOffsets);
	EXPECT_EQ(offsetArray->elementAt(0, 2), 3);
	EXPECT_EQ(offsetArray->elementAt(1, 0), 1);
	EXPECT_EQ(offsetArray->elementAt(2, 1), 2);
}
