#include "../../includes/CLArray.h"
#include "../../includes/PackedArray.h"
#include "../../includes/RowPaddedArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/IOUtils.h"
#include "../../includes/globals.h"
#include "../../includes/cltune.h"

#include <iostream>
#include <vector>
#include <string>
#include <chrono>

const std::string KERNEL = "../kernels/clause_inspection.cl";
const std::string KERNEL_NAME = "clause_inspection";
const std::string REF_KERNEL = "../kernels/clause_inspection_ref.cl";
const std::string REF_KERNEL_NAME = "clause_inspection_ref";

const std::string DATA = "../data/small_clause.test";
const std::string ASSIGNMENT_DATA = "../data/assign.test";

const std::vector<size_t> GROUP_SIZES = {32};

const std::string OUTPUT_JSON_FILE = "bin/tuner_result.json";
std::vector<TunerResult> tunerResults;

void tuneKernel(std::vector<std::vector<int32_t>>& clauses,
  ArrayConfig2D& clausesConfig,
  std::vector<int32_t>& assignments,
  ArrayConfig1D& assignmentsConfig) {
  const size_t M = clauses.size();
  std::vector<int32_t> clauseLengths(M);

  for (int i = 0; i < M; i++) {
    clauseLengths[i] = clauses.at(i).size();
  }

  std::vector<int32_t> target = clauseInspection(clauses, assignments);

  for (auto groupSize : GROUP_SIZES) {
  	for (auto assignmentsBitSize : assignmentsConfig.bitSizes) {
  		for (auto assignmentsPrefetch : assignmentsConfig.prefetches) {
        for (auto clausesBitSize : clausesConfig.bitSizes) {
          for (auto clausesPrefetch : clausesConfig.prefetches) {
  			    for (auto clausesTransform : clausesConfig.transforms) {
              std::cout << "TUNING PARAMETERS" << std::endl;
              std::cout << "Assignments: bitsize: " << assignmentsBitSize << " Prefetch: " << assignmentsPrefetch << std::endl;
              std::cout << "Clauses: bitsize: " << clausesBitSize << " Prefetch: " << clausesPrefetch << " Transform: " << clausesTransform << std::endl;
      				std::string kernelHeader = "";
              std::vector<int32_t> result(M);
              std::unique_ptr<CLArray> clauseDB;
              
              if (clausesTransform == ROW_MAJOR) {
                clauseDB = std::unique_ptr<RowPaddedArray>(new RowPaddedArray("clauses", clausesBitSize, clausesPrefetch, clauses));
              } else if (clausesTransform == COL_MAJOR) {
                // TODO
              } else if (clausesTransform == OFFSET) {
                // TODO
              } else if (clausesTransform == MULTI_PAGE) {
                // TODO
              }
              
              std::unique_ptr<PackedArray> assignmentsArray(new PackedArray("assignments", assignmentsBitSize, assignmentsPrefetch, assignments));

              kernelHeader += clauseDB->generateOpenCLCode();
              kernelHeader += assignmentsArray->generateOpenCLCode();

              std::cout << "GENERATED OPENCL HEADER" << std::endl;
              std::cout << kernelHeader;

              std::string kernel = appendKernelHeader(KERNEL, kernelHeader);
              cltune::Tuner tuner(size_t{PLATFORM_ID}, size_t{DEVICE_ID});
              tuner.AddKernelFromString(kernel, KERNEL_NAME, {M}, {groupSize});
              tuner.SetReference({REF_KERNEL}, REF_KERNEL_NAME, {M}, {1});
              const auto startTime = std::chrono::steady_clock::now();
              tuner.AddArgumentScalar((int)M);
              tuner.AddArgumentInput(clauseDB->getArray());
              tuner.AddArgumentInput(assignmentsArray->getArray());
              tuner.AddArgumentInput(clauseLengths);
              tuner.AddArgumentInput(target);
              tuner.AddArgumentOutput(result);
              const auto cpuTime = std::chrono::steady_clock::now() - startTime;
              const auto dataTransferTime = std::chrono::duration<float,std::milli>(cpuTime).count();
              tuner.SetNumRuns(NUMRUNS);
              tuner.Tune();
              tuner.PrintJSON(OUTPUT_JSON_FILE, {});

              // Store result
              TunerResult tunerResult;
              tunerResult.workGroupSize = groupSize;
              tunerResult.bitSize = assignmentsBitSize;
              tunerResult.prefetch = assignmentsPrefetch;
              tunerResult.transform = "Row";
              tunerResult.dataTransferTime = dataTransferTime;
              tunerResult.executionTime = getExecutionResult(OUTPUT_JSON_FILE);
              tunerResults.push_back(tunerResult);
            }
          }
  			}
  		}
  	}
  }

  std::cout << "Summary:\nGrpSize\tBitsize\tPrefch\tTransform\tTransfer\tExecution\tTotal" << std::endl;
  for (size_t i = 0; i < tunerResults.size(); i++) {
    std::cout << tunerResults.at(i).workGroupSize << "\t"
    << tunerResults.at(i).bitSize << "\t"
    << tunerResults.at(i).prefetch << "\t"
    << tunerResults.at(i).transform << "\t\t"
    << tunerResults.at(i).dataTransferTime << "\t\t"
    << tunerResults.at(i).executionTime << "\t\t"
    << (tunerResults.at(i).dataTransferTime + tunerResults.at(i).executionTime) << std::endl;
  }
}

int main(int argc, char *argv[]) {
	std::vector<std::vector<int32_t>> clauses = readMatrixFromFile(DATA);
  std::vector<int32_t> assignments = readMatrixFromFile(ASSIGNMENT_DATA)[0];

  ArrayConfig2D clausesConfig;
  clausesConfig.bitSizes = {32};
  clausesConfig.prefetches = {false};
  clausesConfig.transforms = {ROW_MAJOR}; // COL_MAJOR, OFFSET, MULTI_PAGE

  ArrayConfig1D assignmentsConfig;
  assignmentsConfig.bitSizes = {2, 32};
  assignmentsConfig.prefetches = {false};

  tuneKernel(clauses,
    clausesConfig,
    assignments,
    assignmentsConfig);
}
