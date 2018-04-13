#ifndef __ROW_PADDING_ARRAY_H__
#define __ROW_PADDING_ARRAY_H__

#include "CLArray.h"
#include "PackedArray.h"

#include <string>
#include <vector>

class RowPaddedArray : public CLArray {
	public:
	  RowPaddedArray(std::string name, int bitSize, bool prefetch, std::vector<std::vector<int32_t>> m);
	  virtual std::string generateOpenCLCode();
	  virtual std::vector<int32_t> getArray();
	  virtual int32_t elementAt(const int i, const int j);

	private:
		virtual std::vector<int32_t> transform(const std::vector<std::vector<int32_t>>& m);

		int height_;
		int width_;
		std::unique_ptr<PackedArray> packedArray_;
};

#endif
