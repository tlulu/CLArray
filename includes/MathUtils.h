#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include <math.h>
#include <vector>

size_t ceilDiv(const size_t x, const size_t y);
int getMaxValue(int numBits);
std::vector<int32_t> clauseInspection(std::vector<std::vector<int32_t>>& matrix, std::vector<int32_t>& assignments);

#endif
