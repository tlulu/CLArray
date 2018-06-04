#include "../../includes/MathUtils.h"
#include "../../includes/globals.h"

#include "gtest/gtest.h"
#include <iostream>
#include <vector>
#include <map>

void testMultipageTransformation(std::vector<std::vector<int32_t>>& m, 
	std::vector<std::vector<int32_t>>& expected, Transform transform) {
	std::map<int, std::vector<std::vector<int32_t>>> multiPages = getMultiPages(m);
	std::map<int, std::unique_ptr<CLArray>> arrMap = transformToMultiPage(multiPages, "A", 32, false, transform, 1);

	EXPECT_EQ(arrMap.size(), 3);
	EXPECT_EQ(arrMap[5]->getArray(), expected[0]);
	EXPECT_EQ(arrMap[2]->getArray(), expected[1]);
	EXPECT_EQ(arrMap[1]->getArray(), expected[2]);
}

TEST(MultipageTest, rowMajor) {
	std::vector<std::vector<int32_t>> m = {{1, 2, 3, 4, 5}, {1, 2}, {1}, {3, 4}, {6, 7, 8, 9, 10}};
	std::vector<std::vector<int32_t>> expected = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
																								{1, 2, 3, 4},
																								{1}};

	testMultipageTransformation(m, expected, Transform::ROW_MAJOR);
}

TEST(MultipageTest, colMajor) {
	std::vector<std::vector<int32_t>> m = {{1, 2, 3, 4, 5}, {1, 2}, {1}, {3, 4}, {6, 7, 8, 9, 10}};
	std::vector<std::vector<int32_t>> expected = {{1, 6, 2, 7, 3, 8, 4, 9, 5, 10},
																								{1, 3, 2, 4},
																								{1}};
	
	testMultipageTransformation(m, expected, Transform::COL_MAJOR);
}
