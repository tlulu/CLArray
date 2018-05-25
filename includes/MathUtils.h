#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include <math.h>
#include <vector>
#include <map>

size_t ceilDiv(const size_t x, const size_t y);
int getMaxValue(int numBits);
void addPadding(std::vector<std::vector<int32_t>>* m);
std::vector<int32_t> clauseInspection(std::vector<std::vector<int32_t>>& matrix, std::vector<int32_t>& assignments);
std::map<int, std::vector<int32_t>> clauseInspectionMulti(std::map<int, std::vector<int32_t>>& matrix, std::vector<int32_t>& assignments);
std::map<int, std::vector<int32_t>> transformToMultiPage(std::vector<std::vector<int32_t>>& in);

#endif
