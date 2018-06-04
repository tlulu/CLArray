#ifndef __PACKED_ARRAY_H__
#define __PACKED_ARRAY_H__

#include "CLArray.h"

#include <string>
#include <vector>

class PackedArray : public CLArray {
	public:
	  PackedArray(std::string name, int bitSize, bool prefetch, 
	  	std::vector<int32_t> a, int workgroupSize = 1);
	  virtual std::string generateOpenCLCode();
	  virtual std::vector<int32_t> getArray(); // Returns the packed array.
	  virtual int numElements();

	  int32_t elementAt(const int index);
	  std::vector<int32_t> unpack();

	private:
	 	std::vector<int32_t> pack(const std::vector<int32_t>& a);
	 	int getRemainingWordSize(const int index);
	 	int getPhysicalIndex(const int index);
	 	std::string generateInitCode();
	 	std::string generateAccessorCode();
	 	std::string generatePackingAccessorCode();
	 	std::string generatePrefetchingCode();

	 	// Packing
	 	int bitSize_;
	 	int numElements_;
	 	int numCellsPerWord_;
	 	int cellMask_;

	 	// Prefetching
	 	bool prefetch_;
	 	int workgroupSize_;

	  std::vector<int32_t> array_;
};

#endif
