#include "../../includes/globals.h"
#include "../../includes/MathUtils.h"
#include "../../includes/RowPaddedArray.h"
#include "../../includes/ColPaddedArray.h"

#include <assert.h>

#include <iostream>

size_t ceilDiv(const size_t x, const size_t y) {
  return 1 + ((x - 1) / y);
}

int getMaxValue(int numBits) {
  if (numBits >= 32) {
    return -1;
  }
	return (1 << numBits) - 1;
}

void addPadding(std::vector<std::vector<int32_t>>* m) {
  // Determine longest column.
  int maxCol = -1;
  for (int i = 0; i < m->size(); i++) {
    maxCol = std::max(maxCol, (int)m->at(i).size());
  }

  const int padding = 0;
  for (int i = 0; i < m->size(); i++) {
    for (int j = 0; j < maxCol; j++) {
      if (j >= m->at(i).size()) {
        m->at(i).push_back(padding);
      }
    }
  }
}

void verifyOutput(std::vector<int32_t>& expected, std::vector<int32_t>& actual) {
  assert(actual.size() != 0);

  if (expected.size() != actual.size()) {
    std::cout << "expected size: " << expected.size() << ", actual size: " << actual.size() << std::endl;
  }
  assert(expected.size() == actual.size());
  for (int i = 0; i < expected.size(); i++) {
    if (expected.at(i) != actual.at(i)) {
      std::cout << "row: " << i << " expected: " << expected.at(i) << ", actual: " << actual.at(i) << std::endl;
    }
    assert(expected.at(i) == actual.at(i));
  }
}

int getMaxWidth(std::vector<std::vector<int32_t>>& m) {
  int max = 0;
  for (int i = 0; i < m.size(); i++) {
    max = std::max(max, (int)(m.at(i).size()));
  }
  return max;
}

int getNumElements(std::vector<std::vector<int32_t>>& m) {
  int count = 0;
  for (int i = 0; i < m.size(); i++) {
    count += m.at(i).size();
  }
  return count;
}

std::vector<std::vector<int32_t>> initMatrix(int rows, int cols, int max) {
  std::vector<std::vector<int32_t>> matrix;
  int c = 1;
  for (int i = 0; i < rows; i++) {
    std::vector<int32_t> vec;
    for (int j = 0; j < cols; j++) {
      vec.push_back(c % max);
      c++;
    }
    matrix.push_back(vec);
  }
  return matrix;
}

std::map<int, std::vector<std::vector<int32_t>>> getMultiPages(std::vector<std::vector<int32_t>>& in) {
  std::map<int, std::vector<int32_t>> sizeToRowMap;
  for (int i = 0; i < in.size(); i++) {
    unsigned int size = in.at(i).size();
    sizeToRowMap[size].push_back(i);
  }

  std::map<int, std::vector<std::vector<int32_t>>> multiPages;
  for (auto it = sizeToRowMap.begin(); it != sizeToRowMap.end(); ++it) {
    unsigned int size = it->first;
    std::vector<int32_t> rows = it->second;
    for (int i = 0; i < rows.size(); i++) {
      multiPages[size].push_back(in.at(rows.at(i)));
    }
  }
  return multiPages;
}

std::map<int, std::unique_ptr<CLArray>> transformToMultiPage(std::map<int, std::vector<std::vector<int32_t>>>& multiPages, 
  std::string name, int bitSize, bool prefetch, Transform transform, int workGroupSize) {
  std::map<int, std::unique_ptr<CLArray>> out;
  for (auto it = multiPages.begin(); it != multiPages.end(); ++it) {
    unsigned int size = it->first;

    std::unique_ptr<CLArray> clArray;
    if (transform == Transform::ROW_MAJOR) {
      clArray = std::unique_ptr<RowPaddedArray>(new RowPaddedArray(name, bitSize, prefetch, it->second, workGroupSize));
    } else if (transform == Transform::COL_MAJOR) {
      clArray = std::unique_ptr<ColPaddedArray>(new ColPaddedArray(name, bitSize, prefetch, it->second, workGroupSize));
    } 
    out[size] = std::move(clArray);
  }

  return out;
}

std::vector<int32_t> clauseInspectionTarget(std::vector<std::vector<int32_t>>& matrix, std::vector<int32_t>& assignments) {
  std::vector<int32_t> result(matrix.size());
  for (int i = 0; i < matrix.size(); i++) {
    int clauseResult = UNRES;
    int count = 0;

    for (int j = 0; j < matrix.at(i).size(); j++) {
      int lit = matrix.at(i).at(j);
      int val = assignments.at(lit);
      if (val == UNDEF) {
        count++;
      } else if (val == TRUE) {
        result[i] = SAT;
        break;
      }
    }

    if (result[i] == SAT) continue;
    
    // Return clause state based on count
    if (count == matrix.at(i).size()) {
      clauseResult = WASTE;
    } else if (count == 0) {
      clauseResult = CONFLICT;
    } else if (count == 1) {
      clauseResult = UNIT;
    } else {
      clauseResult = UNRES;
    }
    result[i] = clauseResult;
  }
  return result;
}

std::vector<int32_t> hadamardTarget(std::vector<std::vector<int32_t>>& m1, 
  std::vector<std::vector<int32_t>>& m2) {
  std::vector<int32_t> target;
  for (int i = 0; i < m1.size(); i++) {
    for (int j = 0; j < m1.at(i).size(); j++) {
      target.push_back(m1.at(i).at(j) * m2.at(i).at(j));
    }
  }
  return target;
}

std::vector<int32_t> getMultiplicationTarget(std::vector<std::vector<int32_t>>& m1, 
  std::vector<std::vector<int32_t>>& m2) {
  std::vector<int32_t> target;
  const int M = m1.size();
  const int K = m1.at(0).size();
  const int N = m2.at(0).size();
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      int acc = 0;
      for (int k = 0; k < K; k++) {
        acc += m1.at(i).at(k) * m2.at(k).at(j);
      }
      target.push_back(acc);
    }
  }
  return target;
}
