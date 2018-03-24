#ifndef __PACKED_ARRAY_H__
#define __PACKED_ARRAY_H__

#include "BaseArray.h"

#include <string>
#include <vector>

class PackedArray : public BaseArray {
	public:
	  PackedArray(std::string name, int bitSize, std::vector<uint32_t> a);
	  std::vector<uint32_t> getPackedArray();
	  uint32_t getElement(const int index);
	  std::vector<uint32_t> unpack();

	  virtual std::string generateAccessorsAndSetters();
	  virtual std::string generatePrefetch();

	private:
	 	std::vector<uint32_t> pack(const std::vector<uint32_t>& a);

	 	int bitSize_;
	 	int numElements_;
	  std::vector<uint32_t> packedArray_;
};

#endif
