#include "../../includes/PackedArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/globals.h"

#include "gtest/gtest.h"
#include <vector>

void packAndUnPack(const std::vector<uint32_t>& vec, const int bitSize, std::vector<uint32_t>& out) {
	std::unique_ptr<PackedArray> packedArray(new PackedArray("packed", bitSize, vec));

	std::vector<uint32_t> packed = packedArray->getPackedArray();
	ASSERT_EQ(packed.size(), ceilDiv(vec.size(),  MAX_BITSIZE / bitSize));
	out = packedArray->unpack();
	ASSERT_EQ(out.size(), vec.size());
}

TEST(PackedArrayTest, smallIntsOneWord) {
	int bitSize = 2;
	std::vector<uint32_t> vec = {1, 2, 3, 1, 3};
	std::vector<uint32_t> unpacked;

	packAndUnPack(vec, bitSize, unpacked);

	EXPECT_EQ(vec, unpacked);
}

TEST(PackedArrayTest, smallIntsMultipleWords) {
	int bitSize = 2;
	std::vector<uint32_t> vec = {1, 2, 3, 1, 3, 2, 1, 0, 3, 2, 1, 3, 1, 1, 3, 2, 2, 2, 0, 1};
	std::vector<uint32_t> unpacked;

	packAndUnPack(vec, bitSize, unpacked);

	EXPECT_EQ(vec, unpacked);
}

TEST(PackedArrayTest, smallestIntsOneWord) {
	int bitSize = 1;
	std::vector<uint32_t> vec = {1, 0, 0, 1, 1};
	std::vector<uint32_t> unpacked;

	packAndUnPack(vec, bitSize, unpacked);

	EXPECT_EQ(vec, unpacked);
}
