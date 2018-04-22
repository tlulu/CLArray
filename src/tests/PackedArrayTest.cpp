#include "../../includes/PackedArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/globals.h"

#include "gtest/gtest.h"
#include <vector>
#include <iostream>

void packAndUnPack(const std::vector<int32_t>& vec, const int bitSize, std::vector<int32_t>& out) {
	std::unique_ptr<PackedArray> packedArray(new PackedArray("packed", bitSize, false, vec));

	std::vector<int32_t> packed = packedArray->getArray();
	ASSERT_EQ(packed.size(), ceilDiv(vec.size(),  MAX_BITSIZE / bitSize));
	out = packedArray->unpack();
	ASSERT_EQ(out.size(), vec.size());
}

TEST(PackedArrayTest, base) {
	int bitSize = 32;
	std::vector<int32_t> vec = {1, 0, 3, 1, 3};
	std::unique_ptr<PackedArray> packedArray(new PackedArray("packed", bitSize, false, vec));

	EXPECT_EQ(vec, packedArray->getArray());
}

TEST(PackedArrayTest, smallIntsOneWord) {
	int bitSize = 2;
	std::vector<int32_t> vec = {1, 2, 3, 1, 3};
	std::vector<int32_t> unpacked;

	packAndUnPack(vec, bitSize, unpacked);

	EXPECT_EQ(vec, unpacked);
}

TEST(PackedArrayTest, smallIntsMultipleWords) {
	int bitSize = 2;
	std::vector<int32_t> vec = {1, 2, 3, 1, 3, 2, 1, 0, 3, 2, 1, 3, 1, 1, 3, 2, 2, 2, 0, 1};
	std::vector<int32_t> unpacked;

	packAndUnPack(vec, bitSize, unpacked);

	EXPECT_EQ(vec, unpacked);
}

TEST(PackedArrayTest, smallestIntsOneWord) {
	int bitSize = 1;
	std::vector<int32_t> vec = {1, 0, 0, 1, 1};
	std::vector<int32_t> unpacked;

	packAndUnPack(vec, bitSize, unpacked);

	EXPECT_EQ(vec, unpacked);
}

TEST(PackedArrayTest, elementAt) {
	int bitSize = 2;
	std::vector<int32_t> vec = {1, 2, 3, 1, 3, 2, 1, 0, 3, 2, 1, 3, 1, 1, 3, 2, 2, 2, 0, 1};
	std::unique_ptr<PackedArray> packedArray(new PackedArray("packed", bitSize, false, vec));

	EXPECT_EQ(packedArray->getArray().size(), 2);
	EXPECT_EQ(packedArray->elementAt(0), 1);
	EXPECT_EQ(packedArray->elementAt(8), 3);
	EXPECT_EQ(packedArray->elementAt(16), 2);
	EXPECT_EQ(packedArray->elementAt(17), 2);
	EXPECT_EQ(packedArray->elementAt(19), 1);
}
