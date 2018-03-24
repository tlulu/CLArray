#include "../../includes/MathUtils.h"

size_t ceilDiv(const size_t x, const size_t y) {
  return 1 + ((x - 1) / y);
}

int getMaxValue(int numBits) {
	return (int)pow(2, numBits) - 1;
}
