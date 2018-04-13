#include "../../includes/BaseArray.h"
#include "../../includes/PackedArray.h"
#include "../../includes/RowPaddedArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/IOUtils.h"
#include "../../includes/globals.h"
#include "../../includes/cltune.h"

#include <iostream>
#include <vector>
#include <string>

const std::string KERNEL = "../kernels/clause_inspection.cl";
const std::string KERNEL_NAME = "clause_inspection";
const std::string REF_KERNEL = "../kernels/clause_inspection_ref.cl";
const std::string REF_KERNEL_NAME = "clause_inspection_ref";

const std::string DATA = "../data/small_clause.test";
const std::string ASSIGNMENT_DATA = "../data/assign.test";

const std::vector<size_t> GROUP_SIZES = {16, 32};

void tuneKernel(std::vector<std::vector<int32_t>>& clauses, 
  std::vector<int32_t>& assignments,
  ArrayConfig2D& clausesConfig) {
  const size_t M = clauses.size();
  std::vector<int32_t> clauseLengths(M);

  for (int i = 0; i < M; i++) {
    clauseLengths[i] = clauses.at(i).size();
  }

  std::vector<int32_t> target = clauseInspection(clauses, assignments);

  for (auto groupSize : GROUP_SIZES) {
  	for (auto bitSize : clausesConfig.bitSizes) {
  		for (auto prefetch : clausesConfig.prefetches) {
  			for (auto transform : clausesConfig.transforms) {
  				std::string kernelHeader = "";
          std::vector<int32_t> result(M);
          std::unique_ptr<BaseArray> clauseDB;
          
          if (transform == ROW_MAJOR) {
            clauseDB = std::unique_ptr<RowPaddedArray>(new RowPaddedArray("clause", bitSize, false, clauses));
          } else if (transform == COL_MAJOR) {
            // TODO
          } else if (transform == OFFSET) {
            // TODO
          } else if (transform == MULTI_PAGE) {
            // TODO
          }
          
          std::unique_ptr<PackedArray> lengthsArray(new PackedArray("length", bitSize, prefetch, clauseLengths));
          std::unique_ptr<PackedArray> assignmentsArray(new PackedArray("assign", bitSize, prefetch, assignments));

          kernelHeader += clauseDB->generateOpenCLCode();
          kernelHeader += assignmentsArray->generateOpenCLCode();
          kernelHeader += lengthsArray->generateOpenCLCode();

          std::cout << "GENERATED OPENCL HEADER" << std::endl;
          std::cout << kernelHeader;

          std::string kernel = appendKernelHeader(KERNEL, kernelHeader);
          cltune::Tuner tuner(size_t{PLATFORM_ID}, size_t{DEVICE_ID});
          tuner.AddKernelFromString(kernel, KERNEL_NAME, {M}, {groupSize});
          tuner.SetReference({REF_KERNEL}, REF_KERNEL_NAME, {M}, {1});     
          // TODO start data transfer time
          tuner.AddArgumentScalar((int)M);
          tuner.AddArgumentInput(clauseDB->getArray());
          tuner.AddArgumentInput(assignmentsArray->getArray());
          tuner.AddArgumentInput(lengthsArray->getArray());
          tuner.AddArgumentInput(target);
          tuner.AddArgumentOutput(result);
          // TODO end data transfer time
          tuner.SetNumRuns(NUMRUNS);
          tuner.Tune();

          // TODO output timing results
  			}
  		}
  	}
  }
}

int main(int argc, char *argv[]) {
	std::vector<std::vector<int32_t>> matrix = readMatrixFromFile(DATA);
  std::vector<int32_t> assignments = readMatrixFromFile(ASSIGNMENT_DATA)[0];

  ArrayConfig2D clausesConfig;
  clausesConfig.bitSizes = {32};
  clausesConfig.prefetches = {false};
  clausesConfig.transforms = {ROW_MAJOR}; // COL_MAJOR, OFFSET, MULTI_PAGE

  tuneKernel(matrix, assignments, clausesConfig);
}
