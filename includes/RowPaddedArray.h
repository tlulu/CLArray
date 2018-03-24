#ifndef __ROW_PADDING_ARRAY_H__
#define __ROW_PADDING_ARRAY_H__

#include "BaseArray.h"

#include <string>
#include <vector>

class RowPaddedArray : public BaseArray {
	public:
	  RowPaddedArray(std::string name, std::vector<std::vector<uint32_t>> m);
	  std::vector<uint32_t> get1DArray();
	  uint32_t getElement(const int i, const int j);

	  virtual std::string generateAccessorsAndSetters();
	  virtual std::string generatePrefetch();

	private:
		std::vector<uint32_t> transform(const std::vector<std::vector<uint32_t>>& m);

		int height_;
		int width_;
	  std::vector<uint32_t> array_;
};

#endif
