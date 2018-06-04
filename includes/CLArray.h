#ifndef __CL_ARRAY_H__
#define __CL_ARRAY_H__

#include <string>
#include <vector>

class CLArray {
	public:
  	CLArray(std::string name);
  	virtual ~CLArray();
  	virtual std::string generateOpenCLCode() = 0;
  	virtual std::vector<int32_t> getArray() = 0;
  	virtual int numElements() = 0; // Number of logical elements in the array.

  protected:
  	std::string getName();
 
	private:
 		std::string name_;
};

#endif
