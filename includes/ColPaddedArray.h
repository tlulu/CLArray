#ifndef __COL_PADDED_ARRAY_H__
#define __COL_PADDED_ARRAY_H__

#include "Array2D.h"
#include "CLArray.h"
#include "PackedArray.h"

#include <string>
#include <vector>

class ColPaddedArray : public CLArray, Array2D {
	public:
	  ColPaddedArray(std::string name, int bitSize, bool prefetch, std::vector<std::vector<int32_t>> m, 
	  	int workgroupSizeX = 1, int workgroupSizeY = 1);
	  virtual std::string generateOpenCLCode();
	  virtual std::vector<int32_t> getArray();
	  virtual int32_t elementAt(const int i, const int j);
	  int getWidth();

	private:
		virtual std::vector<int32_t> transform(const std::vector<std::vector<int32_t>>& m);

		int height_;
		int width_;
		std::unique_ptr<PackedArray> packedArray_;
};

#endif
