#ifndef __BASE_ARRAY_H__
#define __BASE_ARRAY_H__

#include <string>
#include <vector>

class BaseArray {
	public:
  	BaseArray(std::string name);
  
  	virtual std::string generateAccessorsAndSetters() = 0;
  	virtual std::string generatePrefetch() = 0;

  protected:
  	std::string getName();
 
	private:
 		std::string name_;
};

#endif
