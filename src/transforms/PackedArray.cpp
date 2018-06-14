#include "../../includes/PackedArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/IOUtils.h"
#include "../../includes/globals.h"

#include <sstream>

PackedArray::PackedArray(std::string name, int bitSize, bool prefetch, 
	std::vector<int32_t> a, int workgroupSize)
  : CLArray(name), 
  	bitSize_(bitSize),
  	numElements_(a.size()),
  	numCellsPerWord_(MAX_BITSIZE / bitSize),
  	cellMask_(getMaxValue(bitSize)),
  	prefetch_(prefetch),
  	workgroupSize_(workgroupSize) {
  		if (bitSize > MAX_BITSIZE) {
  			throw std::runtime_error("Cannot select a bit size greater than 32");
  		}
  		if (MAX_BITSIZE % bitSize != 0) {
  			throw std::runtime_error("Bit size must divide 32");
  		}
  		if (prefetch_ && workgroupSize_ == 1) {
  			throw std::runtime_error("Prefetch is ON but workgroup size is 1");
  		}

  		array_ = pack(a);
  	}

std::string PackedArray::generateOpenCLCode() {
	std::stringstream ss;

	ss << generateInitCode();

	if (bitSize_ == MAX_BITSIZE) {
		ss << generateAccessorCode();
	} else {
		ss << generatePackingAccessorCode();
	}

	if (prefetch_) {
		ss << generatePrefetchingCode();
	} else {
		// Used for testing purposes, not in the actual API.
		ss << "#define INIT_LOCAL_name(global, local) __global const uint* (local) = (global);" << std::endl;
	}

	std::string output = ss.str();
  output = replaceString(output, "name", getName());
  output = replaceString(output, "SCOPE", prefetch_ ? "__local" : "__global");
  return output;
}

std::string PackedArray::generateInitCode() {
	std::stringstream ss;

	// Bit-packing constants
	ss << "#define name_bit_size " << bitSize_ << std::endl;
	ss << "#define name_num_cells_per_word " << numCellsPerWord_ << std::endl;
	ss << "#define name_cell_mask " << cellMask_ << std::endl;
	ss << std::endl;

	// Bit-packing helpers
	ss << "int name_get_remaining_word_size(const int index) {" << std::endl;
	ss << "  return 32 - name_bit_size * ((index % name_num_cells_per_word) + 1);" << std::endl;
	ss << "}" << std::endl;
	ss << std::endl;

	ss << "int name_get_physical_index(const int logical_index) {" << std::endl;
	ss << "  return logical_index / name_num_cells_per_word;" << std::endl;
	ss << "}" << std::endl;
	ss << std::endl;

	return ss.str();
}

std::string PackedArray::generateAccessorCode() {
	std::stringstream ss;

	ss << "int name_get(SCOPE const int* arr, const int index) {" << std::endl;
	ss << "  return arr[index];" << std::endl;
	ss << "}" << std::endl;
	ss << std::endl;

	return ss.str();
}

std::string PackedArray::generatePackingAccessorCode() {
	std::stringstream ss;

	ss << "int name_get(SCOPE const int* arr, const int index) {" << std::endl;
	ss << "  int physical_index = name_get_physical_index(index);" << std::endl;
	ss << "  int word = arr[physical_index];" << std::endl;
	ss << "  int shift = name_get_remaining_word_size(index);" << std::endl;
	ss << "  return (word >> shift) & name_cell_mask;" << std::endl;
	ss << "}" << std::endl;
	ss << std::endl;

	return ss.str();
}

std::string PackedArray::generatePrefetchingCode() {
	std::stringstream ss;

	int loopBoundFloor = (int) floor((float) array_.size() / workgroupSize_);
  int loopBoundCeil = (int) ceil((float) array_.size() / workgroupSize_);

  // Prefetching function
  ss << "void name_prefetch(__global const int* const ga, __local int* la) {" << std::endl;
  ss << "  const int local_size = get_local_size(0);" << std::endl;
  ss << "  const int thread_id = get_local_id(0);" << std::endl;

  ss << "  for (int loop = 0; loop < LOOP_BOUND_FLOOR; loop++) {" << std::endl;
  ss << "    const int index = loop * local_size + thread_id;" << std::endl;
  ss << "    la[index] = ga[index];" << std::endl;
  ss << "  }" << std::endl;

  // Second loop to check that we are in bounds.
  // There will be at most one iteration of this loop.
  if (loopBoundFloor < loopBoundCeil) {
	  ss << "  for (int loop = LOOP_BOUND_FLOOR; loop < LOOP_BOUND_CEIL; loop++) {" << std::endl;
	  ss << "    const int index = loop * local_size + thread_id;" << std::endl;
	  ss << "    if (index < ARR_LENGTH) {" << std::endl;
	  ss << "      la[index] = ga[index];" << std::endl;
	  ss << "    }" << std::endl;
	  ss << "  }" << std::endl;
  }

  ss << "  barrier(CLK_LOCAL_MEM_FENCE);" << std::endl;
  ss << "}" << std::endl;
  ss << std::endl;

  // Macro definition
	ss << "#define INIT_LOCAL_name(global, local) ";
	ss << "__local int (local)[ARR_LENGTH]; ";
  ss << "name_prefetch((global), (local));" << std::endl;
  ss << std::endl;

	std::string output = ss.str();
	output = replaceString(output, "ARR_LENGTH", std::to_string(array_.size()));
  output = replaceString(output, "LOOP_BOUND_FLOOR", std::to_string(loopBoundFloor));
  output = replaceString(output, "LOOP_BOUND_CEIL", std::to_string(loopBoundCeil));
  return output;
}

std::vector<int32_t> PackedArray::getArray() {
	return array_;
}

int PackedArray::numElements() {
	return numElements_;
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

	return words;
}

int PackedArray::getRemainingWordSize(const int index) {
	return MAX_BITSIZE - bitSize_ * ((index % numCellsPerWord_) + 1);
}

int PackedArray::getPhysicalIndex(const int index) {
	return index / numCellsPerWord_;
}
