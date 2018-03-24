#include "../../includes/PackedArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/globals.h"

PackedArray::PackedArray(std::string name, int bitSize, std::vector<uint32_t> a)
  : BaseArray(name), 
  	bitSize_(bitSize),
  	numElements_(a.size()) {
  		if (bitSize > MAX_BITSIZE) {
  			throw std::runtime_error("Cannot select a bitsize greater than 32");
  		}

  		packedArray_ = pack(a);
  	}

std::vector<uint32_t> PackedArray::getPackedArray() {
	return packedArray_;
}

uint32_t PackedArray::getElement(const int index) {
	// TODO
	return 0;
}

std::vector<uint32_t> PackedArray::unpack() {
	std::vector<uint32_t> unPackedArray;
	const int numCells = MAX_BITSIZE / bitSize_;
	const int mask = getMaxValue(bitSize_);

	int c = 0;
	for (int i = 0; i < numElements_; i++) {
		int shift = MAX_BITSIZE - bitSize_ * ((i + 1) % numCells);
		uint32_t cur = (packedArray_.at(c) >> shift) & mask;
		unPackedArray.push_back(cur);

		if ((i + 1) % numCells == 0) {
			c++;
		}
	}

	return unPackedArray;
}

std::string PackedArray::generateAccessorsAndSetters() {
	// TODO
	return "";
}

std::string PackedArray::generatePrefetch() {
	// TODO
	return "";
}

// Assumes that bitSize is a divisor of MAX_BITSIZE.
std::vector<uint32_t> PackedArray::pack(const std::vector<uint32_t>& a) {
	int numCells = MAX_BITSIZE / bitSize_;
	size_t newArraySize = ceilDiv(a.size(), numCells);
	std::vector<uint32_t> packedArray(newArraySize);

	uint32_t n = 0;
	int c = 0;
	for (int i = 0; i < a.size(); i++) {
		int shift = MAX_BITSIZE - bitSize_ * ((i + 1) % numCells);
		n |= (a[i] << shift);

		if ((i + 1) % numCells == 0) {
			packedArray[c] = n;
			n = 0;
			c++;
		}
	}

	// Mask remaining 32 bits.
	int remainingBits = MAX_BITSIZE - (bitSize_ * (a.size() % numCells));
	uint32_t mask =  ~(getMaxValue(remainingBits));
	packedArray[c] = n & mask;

	return packedArray;
}
