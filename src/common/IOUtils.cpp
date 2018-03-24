#include "../../includes/IOUtils.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>

std::vector<std::vector<int>> readMatrixFromFile(std::string fileName) {
  std::ifstream infile(fileName);
  if (!infile.is_open()) {
    std::cout << "Error while opening file: " << fileName << std::endl;
    exit(EXIT_FAILURE);
  }

  std::vector<std::vector<int>> matrix;
  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream buf(line);
    std::istream_iterator<std::string> beg(buf), end;

    std::vector<std::string> numberStrings(beg, end);
    std::vector<int> vec;
    for (std::vector<std::string>::iterator it = numberStrings.begin();
         it != numberStrings.end(); ++it) {
      vec.push_back(stoi(*it));
    }

    matrix.push_back(vec);
  }

  return matrix;
}
