#ifndef __PACKED_ARRAY_H__
#define __PACKED_ARRAY_H__

#include "BaseArray.h"

#include <string>
#include <vector>

class PackedArray : public BaseArray {
	public:
	  PackedArray(std::string name, int bitSize, bool prefetch, std::vector<int32_t> a);
	  virtual std::string generateOpenCLCode();
	  virtual std::vector<int32_t> getArray(); // Returns the packed array.

	  int32_t elementAt(const int index);
	  std::vector<int32_t> unpack();

	private:
	 	std::vector<int32_t> pack(const std::vector<int32_t>& a);

	 	int bitSize_;
	 	bool prefetch_;
	 	int numElements_;
	  std::vector<int32_t> array_;
};

#endif
