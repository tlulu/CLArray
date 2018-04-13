#include "../../includes/globals.h"
#include "../../includes/MathUtils.h"

size_t ceilDiv(const size_t x, const size_t y) {
  return 1 + ((x - 1) / y);
}

int getMaxValue(int numBits) {
	return (int)pow(2, numBits) - 1;
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

std::vector<int32_t> clauseInspection(std::vector<std::vector<int32_t>>& matrix, std::vector<int32_t>& assignments) {
  std::vector<int32_t> result(matrix.size());
  for (int i = 0; i < matrix.size(); i++) {
    int clause_result = UNRES;
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

    if (result[i] != SAT) {
      // Return clause state based on count
      if (count == matrix.at(i).size()) {
        clause_result = WASTE;
      }
      else if (count == 0) {
        clause_result = CONFLICT;
      }
      else if (count == 1) {
        clause_result = UNIT;
      }
      else {
        clause_result = UNRES;
      }

      result[i] = clause_result;
    }
  }
  return result;
}
