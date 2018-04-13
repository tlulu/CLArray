#include "../../includes/PackedArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/IOUtils.h"
#include "../../includes/globals.h"

#include <sstream>

PackedArray::PackedArray(std::string name, int bitSize, bool prefetch, std::vector<int32_t> a)
  : CLArray(name), 
  	bitSize_(bitSize),
  	prefetch_(prefetch),
  	numElements_(a.size()),
  	numCellsPerSection_(MAX_BITSIZE / bitSize) {
  		if (bitSize > MAX_BITSIZE) {
  			throw std::runtime_error("Cannot select a bitsize greater than 32");
  		}

  		array_ = pack(a);
  	}

std::string PackedArray::generateOpenCLCode() {
	std::stringstream ss;

	if (prefetch_) {
		// TODO
	}

	ss << "int name_get(__global const int* arr, const int index) {" << std::endl;
	ss << "  return arr[index];" << std::endl;
	ss << "}" << std::endl;
	ss << std::endl;

	std::string output = ss.str();
  output = replaceString(output, "name", getName());
  return output;
}

std::vector<int32_t> PackedArray::getArray() {
	return array_;
}

int32_t PackedArray::elementAt(const int index) {
	int physicalIndex = index / numCellsPerSection_;
	int32_t section = array_.at(physicalIndex);
	int subIndex = index % numCellsPerSection_;

	int shift = MAX_BITSIZE - bitSize_ * (subIndex + 1);
	int32_t mask = getMaxValue(bitSize_);
	return (section >> shift) & mask;
}

std::vector<int32_t> PackedArray::unpack() {
	std::vector<int32_t> unPackedArray;
	const int mask = getMaxValue(bitSize_);

	int c = 0;
	for (int i = 0; i < numElements_; i++) {
		int shift = MAX_BITSIZE - bitSize_ * ((i + 1) % numCellsPerSection_);
		int32_t cur = (array_.at(c) >> shift) & mask;
		unPackedArray.push_back(cur);

		if ((i + 1) % numCellsPerSection_ == 0) {
			c++;
		}
	}

	return unPackedArray;
}

// Assumes that bitSize is a divisor of MAX_BITSIZE.
std::vector<int32_t> PackedArray::pack(const std::vector<int32_t>& a) {
	size_t newArraySize = ceilDiv(a.size(), numCellsPerSection_);
	std::vector<int32_t> packedArray(newArraySize);

	int32_t n = 0;
	int c = 0;
	for (int i = 0; i < a.size(); i++) {
		int shift = MAX_BITSIZE - bitSize_ * ((i + 1) % numCellsPerSection_);
		n |= (a[i] << shift);

		if ((i + 1) % numCellsPerSection_ == 0) {
			packedArray[c] = n;
			n = 0;
			c++;
		}
	}

	// Mask remaining 32 bits.
	int remainingBits = MAX_BITSIZE - (bitSize_ * (a.size() % numCellsPerSection_));
	int32_t mask =  ~(getMaxValue(remainingBits));
	packedArray[c] = n & mask;

	return packedArray;
}
