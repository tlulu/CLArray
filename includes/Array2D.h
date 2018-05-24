#ifndef __Array2D_H__
#define __Array2D_H__

#include <vector>

class Array2D {
	public:
		Array2D() {};
		virtual ~Array2D() {};
  	virtual std::vector<int32_t> transform(const std::vector<std::vector<int32_t>>& m) = 0;
  	virtual int32_t elementAt(const int i, const int j) = 0;
};

#endif
