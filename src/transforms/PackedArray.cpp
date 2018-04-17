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
  	numCellsPerWord_(MAX_BITSIZE / bitSize),
  	cellMask_(getMaxValue(bitSize)) {
  		if (bitSize > MAX_BITSIZE) {
  			throw std::runtime_error("Cannot select a bit size greater than 32");
  		}
  		if (MAX_BITSIZE % bitSize != 0) {
  			throw std::runtime_error("Bit size must divide 32");
  		}

  		array_ = pack(a);
  	}

std::string PackedArray::generateOpenCLCode() {
	std::stringstream ss;

	if (prefetch_) {
		// TODO
	}

	// Bit-packing constants
	ss << "#define name_bit_size " << bitSize_ << std::endl;
	ss << "#define name_num_cells_per_word " << numCellsPerWord_ << std::endl;
	ss << "#define name_cell_mask " << cellMask_ << std::endl;
	ss << std::endl;

	// Helper
	ss << "int name_get_remaining_word_size(const int index) {" << std::endl;
	ss << "  return 32 - name_bit_size * ((index % name_num_cells_per_word) + 1);" << std::endl;
	ss << "}" << std::endl;
	ss << std::endl;

	// Accessor
	ss << "int name_get(__global const int* arr, const int index) {" << std::endl;
	ss << "  int physical_index = index / name_num_cells_per_word;" << std::endl;
	ss << "  int word = arr[physical_index];" << std::endl;
	ss << "  int shift = name_get_remaining_word_size(index);" << std::endl;
	ss << "  return (word >> shift) & name_cell_mask;" << std::endl;
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
	int32_t word = array_.at(getPhysicalIndex(index));
	int shift = getRemainingWordSize(index);

	return (word >> shift) & cellMask_;
}

std::vector<int32_t> PackedArray::unpack() {
	std::vector<int32_t> unPackedArray;
	int c = 0;
	for (int i = 0; i < numElements_; i++) {
		int shift = getRemainingWordSize(i);
		int32_t cell = (array_.at(c) >> shift) & cellMask_;
		unPackedArray.push_back(cell);

		if ((i + 1) % numCellsPerWord_ == 0) {
			c++;
		}
	}

	return unPackedArray;
}

std::vector<int32_t> PackedArray::pack(const std::vector<int32_t>& a) {
	if (a.size() == 0) return {};
	
	size_t numWords = ceilDiv(a.size(), numCellsPerWord_);
	std::vector<int32_t> words(numWords, 0);

	// Pack each int into a 32 bit word.
	int c = 0;
	for (int i = 0; i < a.size(); i++) {
		int shift = getRemainingWordSize(i);
		words[c] |= (a[i] << shift);

		if ((i + 1) % numCellsPerWord_ == 0) {
			c++;
		}
	}

	// Mask remaining bits of the last word.
	int remainingWordSize = getRemainingWordSize(a.size());
	int32_t mask =  ~(getMaxValue(remainingWordSize));
	words[numWords - 1] = words[numWords - 1] & mask;

	return words;
}

int PackedArray::getRemainingWordSize(const int index) {
	return MAX_BITSIZE - bitSize_ * ((index % numCellsPerWord_) + 1);
}

int PackedArray::getPhysicalIndex(const int index) {
	return index / numCellsPerWord_;
}
