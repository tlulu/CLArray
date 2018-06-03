#ifndef __OFFSET_ARRAY_H__
#define __OFFSET_ARRAY_H__

#include "Array2D.h"
#include "CLArray.h"
#include "PackedArray.h"

#include <string>
#include <vector>

class OffsetArray : public CLArray, Array2D {
	public:
	  OffsetArray(std::string name, int bitSize, bool prefetch, std::vector<std::vector<int32_t>> m, 
	  	int workgroupSizeX = 1, int workgroupSizeY = 1);
	  virtual std::string generateOpenCLCode();
	  virtual std::vector<int32_t> getArray();
	  virtual int32_t elementAt(const int i, const int j);
	  virtual int numElements();
	  std::vector<int32_t> getOffsets();

	private:
		virtual std::vector<int32_t> transform(const std::vector<std::vector<int32_t>>& m);
		std::vector<int32_t> buildOffsets(const std::vector<std::vector<int32_t>>& m);

		std::unique_ptr<PackedArray> offsets_;
		std::unique_ptr<PackedArray> packedArray_;
};

#endif
