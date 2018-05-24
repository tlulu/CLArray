#include "../../includes/IOUtils.h"
#include "../../includes/json11.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>

std::vector<std::vector<int32_t>> readMatrixFromFile(std::string fileName) {
  std::ifstream infile(fileName);
  if (!infile.is_open()) {
    std::cout << "Error while opening file: " << fileName << std::endl;
    exit(EXIT_FAILURE);
  }

  std::vector<std::vector<int32_t>> matrix;
  std::string line;
  while (std::getline(infile, line)) {
    std::istringstream buf(line);
    std::istream_iterator<std::string> beg(buf), end;

    std::vector<std::string> numberStrings(beg, end);
    std::vector<int32_t> vec;
    for (std::vector<std::string>::iterator it = numberStrings.begin();
         it != numberStrings.end(); ++it) {
      vec.push_back(stoi(*it));
    }

    matrix.push_back(vec);
  }

  return matrix;
}

std::string appendKernelHeader(std::string kernelFile, std::string kernelHeader) {
  std::ifstream kernelFileStream{kernelFile};
  std::string kernel{std::istreambuf_iterator<char>(kernelFileStream), std::istreambuf_iterator<char>()};
  return kernelHeader + kernel;
}

std::string replaceString(std::string subject, 
  const std::string& search, const std::string& replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return subject;
}

double getExecutionResult(std::string jsonFile) {
  std::ifstream ifStreamJsonFile{jsonFile};
  std::string jsonString{std::istreambuf_iterator<char>(ifStreamJsonFile), std::istreambuf_iterator<char>()};
  std::string jsonError;
  json11::Json json = json11::Json::parse(jsonString, jsonError, json11::JsonParse::STANDARD);

  return json["results"][0]["time"].number_value();
}

std::string transformToString(Transform transform) {
  if (transform == ROW_MAJOR) {
    return "row";
  } else if (transform == COL_MAJOR) {
    return "col";
  } else if (transform == OFFSET) {
    return "offset";
  } else if (transform == MULTI_PAGE) {
    return "multipage";
  } else {
    return "none";
  }
}
