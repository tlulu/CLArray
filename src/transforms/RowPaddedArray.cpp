#include "../../includes/RowPaddedArray.h"

RowPaddedArray::RowPaddedArray(std::string name, int bitSize, bool prefetch, std::vector<std::vector<int32_t>> m)
	: BaseArray(name), height_(m.size()) {
      init(&m);

      array_ = transform(m);
    }

std::string RowPaddedArray::generateOpenCLCode() {
  // TODO
  return "";
}

std::vector<int32_t> RowPaddedArray::getArray() {
  return array_;
}

int32_t RowPaddedArray::elementAt(const int i, const int j) {
	return array_.at(i * width_ + j);
}

std::vector<int32_t> RowPaddedArray::transform(const std::vector<std::vector<int32_t>>& m) {
	std::vector<int32_t> a;
  for (size_t i = 0; i < height_; i++) {
    for (size_t j = 0; j < width_; j++) {
      a.push_back(m.at(i).at(j));
    }
  }
  return a;
}

void RowPaddedArray::init(std::vector<std::vector<int32_t>>* m) {
  // Determine longest column.
  width_ = -1;
  for (int i = 0; i < m->size(); i++) {
    width_ = std::max(width_, (int)m->at(i).size());
  }

  const int padding = 0;
  for (int i = 0; i < m->size(); i++) {
    for (int j = 0; j < width_; j++) {
      if (j >= m->at(i).size()) {
        m->at(i).push_back(padding);
      }
    }
  }
}
