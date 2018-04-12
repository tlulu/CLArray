#ifndef __IO_UTILS_H__
#define __IO_UTILS_H__

#include <string>
#include <vector>

std::vector<std::vector<int32_t>> readMatrixFromFile(std::string fileName);
std::string appendKernelHeader(std::string kernelFile, std::string kernelHeader);

#endif
