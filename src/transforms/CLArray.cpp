#include "../../includes/CLArray.h"

CLArray::CLArray(std::string name)
  : name_(name) {}

CLArray::~CLArray() {}

std::string CLArray::getName() {
  return name_;
}
