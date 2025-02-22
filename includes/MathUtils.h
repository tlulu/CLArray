#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include <math.h>
#include <vector>
#include <map>
#include <string>
#include <memory>

#include "globals.h"
#include "CLArray.h"

size_t ceilDiv(const size_t x, const size_t y);
int getMaxValue(int numBits);
void addPadding(std::vector<std::vector<int32_t>>* m);
void verifyOutput(std::vector<int32_t>& expected, std::vector<int32_t>& actual);
int getMaxWidth(std::vector<std::vector<int32_t>>& m);
int getNumElements(std::vector<std::vector<int32_t>>& m);
std::vector<std::vector<int32_t>> initMatrix(int rows, int cols, int max);

// Multipage
std::map<int, std::vector<std::vector<int32_t>>> getMultiPages(std::vector<std::vector<int32_t>>& in);
std::map<int, std::unique_ptr<CLArray>> transformToMultiPage(std::map<int, std::vector<std::vector<int32_t>>>& multiPages, std::string name, int bitSize, bool prefetch, Transform transform, int workGroupSize);

// Clause Inspection
std::vector<int32_t> clauseInspectionTarget(std::vector<std::vector<int32_t>>& matrix, std::vector<int32_t>& assignments);

// Hadamard
std::vector<int32_t> hadamardTarget(std::vector<std::vector<int32_t>>& m1, std::vector<std::vector<int32_t>>& m2);

// Multiplication
std::vector<int32_t> getMultiplicationTarget(std::vector<std::vector<int32_t>>& m1, std::vector<std::vector<int32_t>>& m2);

#endif
