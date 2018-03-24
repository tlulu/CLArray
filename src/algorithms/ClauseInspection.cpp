#include "../../includes/RowPaddedArray.h"
#include "../../includes/IOUtils.h"
#include "../../includes/globals.h"

#include <iostream>
#include <vector>
#include <string>

const std::string KERNEL = "../kernels/clause_inspection.cl";
const std::string KERNEL_NAME = "clause_inspection";
const std::string REF_KERNEL = "../kernels/clause_inspection_ref.cl";
const std::string REF_KERNEL_NAME = "clause_inspection_ref";

const std::string DATA = "../data/small_clause.test";
const std::string ASSIGNMENT_DATA = "../data/assign.test";

void row_major_padding(std::vector<std::vector<int>>& clauses, std::vector<int>& assignments) {
	// TODO
}

int main(int argc, char *argv[]) {
	std::vector<std::vector<int>> matrix = readMatrixFromFile(DATA);
  std::vector<int> assignments = readMatrixFromFile(ASSIGNMENT_DATA)[0];

  row_major_padding(matrix, assignments);
}