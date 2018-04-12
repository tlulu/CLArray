#ifndef __BASE_ARRAY_H__
#define __BASE_ARRAY_H__

#include <string>
#include <vector>

class BaseArray {
	public:
  	BaseArray(std::string name);
  	virtual std::string generateOpenCLCode() = 0;
  	virtual std::vector<int32_t> getArray() = 0;

  protected:
  	std::string getName();
 
	private:
 		std::string name_;
};

#endif
