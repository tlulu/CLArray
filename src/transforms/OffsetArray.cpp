#include "../../includes/OffsetArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/IOUtils.h"

#include <sstream>

OffsetArray::OffsetArray(std::string name, int bitSize, bool prefetch, 
  std::vector<std::vector<int32_t>> m, int workgroupSizeX, int workgroupSizeY)
	: CLArray(name) {
      // If total number of elements are less than 2^16, then we can pack offsets.
      // Offset array size may exceed local memory size if prefetching is turned on.
      offsets_ = std::unique_ptr<PackedArray>{new PackedArray(name + "_offsets", 32, false, buildOffsets(m), workgroupSizeX)};
      packedArray_ = std::unique_ptr<PackedArray>{new PackedArray(name, bitSize, prefetch, transform(m))};
    }

std::string OffsetArray::generateOpenCLCode() {
  std::stringstream ss;
  ss << packedArray_->generateOpenCLCode();
  ss << "#define _2D_name_get(arr, i, j, offsets) name_get(arr, (offsets)[i] + j);" << std::endl;
  ss << std::endl;

  ss << offsets_->generateOpenCLCode();
  ss << std::endl;

  std::string output = ss.str();
  output = replaceString(output, "name", getName());
  return output;
}

std::vector<int32_t> OffsetArray::getArray() {
  return packedArray_->getArray();
}

int OffsetArray::numElements() {
  return packedArray_->numElements();
}

int32_t OffsetArray::elementAt(const int i, const int j) {
	return packedArray_->elementAt(offsets_->elementAt(i) + j);
}

std::vector<int32_t> OffsetArray::getOffsets() {
  return offsets_->getArray();
}

std::vector<int32_t> OffsetArray::transform(const std::vector<std::vector<int32_t>>& m) {
	std::vector<int32_t> a;
  for (size_t i = 0; i < m.size(); i++) {
    for (size_t j = 0; j < m.at(i).size(); j++) {
      a.push_back(m.at(i).at(j));
    }
  }
  return a;
}

std::vector<int32_t> OffsetArray::buildOffsets(const std::vector<std::vector<int32_t>>& m) {
  std::vector<int32_t> offsets;
  int count = 0;
  for (size_t i = 0; i < m.size(); i++) {
    offsets.push_back(count);
    for (size_t j = 0; j < m.at(i).size(); j++) {
      count++;
    }
  }
  offsets.push_back(count);
  return offsets;
}
