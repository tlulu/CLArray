#ifndef __IO_UTILS_H__
#define __IO_UTILS_H__

#include <string>
#include <vector>

#include "globals.h"

std::vector<std::vector<int32_t>> readMatrixFromFile(std::string fileName);
std::string appendKernelHeader(std::string kernelFile, std::string kernelHeader);
std::string replaceString(std::string subject, const std::string& search, const std::string& replace);
double getExecutionResult(std::string jsonFile);
std::string transformToString(Transform transform);

#endif
