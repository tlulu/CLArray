#include "../../includes/OffsetArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/IOUtils.h"

#include <sstream>

OffsetArray::OffsetArray(std::string name, int bitSize, bool prefetch, 
  std::vector<std::vector<int32_t>> m, int workgroupSizeX, int workgroupSizeY)
	: CLArray(name) {
      packedArray_ = std::unique_ptr<PackedArray>{new PackedArray(name, bitSize, prefetch, transform(m))};
    }

std::string OffsetArray::generateOpenCLCode() {
  std::stringstream ss;
  ss << packedArray_->generateOpenCLCode();
  ss << "#define _2D_name_get(arr, i, j, offsets) name_get(arr, (offsets)[i] + j);" << std::endl;
  ss << std::endl;

  std::string output = ss.str();
  output = replaceString(output, "name", getName());
  return output;
}

std::vector<int32_t> OffsetArray::getArray() {
  return packedArray_->getArray();
}

int32_t OffsetArray::elementAt(const int i, const int j) {
	return packedArray_->elementAt(offsets_[i] + j);
}

std::vector<int32_t> OffsetArray::getOffsets() {
  return offsets_;
}

std::vector<int32_t> OffsetArray::transform(const std::vector<std::vector<int32_t>>& m) {
	std::vector<int32_t> a;
  int count = 0;
  for (size_t i = 0; i < m.size(); i++) {
    offsets_.push_back(count);
    for (size_t j = 0; j < m.at(i).size(); j++) {
      a.push_back(m.at(i).at(j));
      count++;
    }
  }
  return a;
}
