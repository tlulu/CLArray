#include "../../includes/ColPaddedArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/IOUtils.h"

#include <sstream>

ColPaddedArray::ColPaddedArray(std::string name, int bitSize, bool prefetch, 
  std::vector<std::vector<int32_t>> m, int workgroupSizeX, int workgroupSizeY)
	: CLArray(name), height_(m.size()) {
      addPadding(&m);

      width_ = m.at(0).size();
      packedArray_ = std::unique_ptr<PackedArray>{new PackedArray(name, bitSize, prefetch, transform(m))};
    }

std::string ColPaddedArray::generateOpenCLCode() {
  std::stringstream ss;
  ss << packedArray_->generateOpenCLCode();
  ss << "#define name_width " << width_ << std::endl;
  ss << "#define name_height " << height_ << std::endl;
  ss << "#define _2D_name_get(arr, i, j) name_get(arr, j * name_height + i);" << std::endl;
  ss << std::endl;

  std::string output = ss.str();
  output = replaceString(output, "name", getName());
  return output;
}

std::vector<int32_t> ColPaddedArray::getArray() {
  return packedArray_->getArray();
}

int32_t ColPaddedArray::elementAt(const int i, const int j) {
	return packedArray_->elementAt(j * height_ + i);
}

int ColPaddedArray::getWidth() {
  return width_;
}

std::vector<int32_t> ColPaddedArray::transform(const std::vector<std::vector<int32_t>>& m) {
	std::vector<int32_t> a;
  for (size_t j = 0; j < width_; j++) {
    for (size_t i = 0; i < height_; i++) {
      a.push_back(m.at(i).at(j));
    }
  }
  return a;
}
