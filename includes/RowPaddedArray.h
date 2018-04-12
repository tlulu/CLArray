#ifndef __ROW_PADDING_ARRAY_H__
#define __ROW_PADDING_ARRAY_H__

#include "BaseArray.h"

#include <string>
#include <vector>

class RowPaddedArray : public BaseArray {
	public:
	  RowPaddedArray(std::string name, int bitSize, bool prefetch, std::vector<std::vector<int32_t>> m);
	  virtual std::string generateOpenCLCode();
	  virtual std::vector<int32_t> getArray();

	  int32_t elementAt(const int i, const int j);

	private:
		std::vector<int32_t> transform(const std::vector<std::vector<int32_t>>& m);
		void init(std::vector<std::vector<int32_t>>* m);

		int height_;
		int width_;
		std::vector<int32_t> array_;
};

#endif
