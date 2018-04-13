#include "../../includes/BaseArray.h"

BaseArray::BaseArray(std::string name)
  : name_(name) {}

BaseArray::~BaseArray() {}

std::string BaseArray::getName() {
  return name_;
}
