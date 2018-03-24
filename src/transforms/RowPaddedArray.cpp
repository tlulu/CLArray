#include "../../includes/RowPaddedArray.h"

RowPaddedArray::RowPaddedArray(std::string name, std::vector<std::vector<uint32_t>> m)
	: BaseArray(name),
		height_(m.size()),
		width_(m.at(0).size()) {
			array_ = transform(m);
  	}

std::vector<uint32_t> RowPaddedArray::get1DArray() {
	return array_;
}

uint32_t RowPaddedArray::getElement(const int i, const int j) {
	return array_.at(i * width_ + j);
}

std::string RowPaddedArray::generateAccessorsAndSetters() {
	// TODO
	return "";
}

std::string RowPaddedArray::generatePrefetch() {
	// TODO
	return "";
}

std::vector<uint32_t> RowPaddedArray::transform(const std::vector<std::vector<uint32_t>>& m) {
	std::vector<uint32_t> a;
  for (size_t i = 0; i < height_; i++) {
    for (size_t j = 0; j < width_; j++) {
      a.push_back(m.at(i).at(j));
    }
  }
  return a;
}
