#include "../../includes/BaseArray.h"

BaseArray::BaseArray(std::string name)
  : name_(name) {}

std::string BaseArray::getName() {
  return name_;
}
