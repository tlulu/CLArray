#include "../../includes/CLArray.h"
#include "../../includes/PackedArray.h"
#include "../../includes/ColPaddedArray.h"
#include "../../includes/RowPaddedArray.h"
#include "../../includes/OffsetArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/IOUtils.h"
#include "../../includes/globals.h"
#include "../../includes/cltune.h"

#include <assert.h>

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

const std::string KERNEL_NAME = "clause_inspection";
const std::string DATA = "../data/clause/small_clause.test";
const std::string ASSIGNMENT_DATA = "../data/clause/assign.test";
const std::string OUTPUT_JSON_FILE = "bin/tuner_result.json";

struct TunerResult {
  int workGroupSize;
  int assignmentsBitSize;
  bool assignmentsPrefetch;
  int clausesBitSize;
  bool clausesPrefetch;
  Transform clausesTransform;
  double executionTime;
  double dataTransferTime;
};

std::vector<TunerResult> tunerResults;

void printResults() {
  const int width = 12;

  std::cout << "Summary:" << std::endl;
  std::cout << std::left << std::setw(width) << "GrpSize";
  std::cout << std::left << std::setw(width) << "(Bitsize";
  std::cout << std::left << std::setw(width) << "Prefch)";
  std::cout << std::left << std::setw(width) << "(Bitsize";
  std::cout << std::left << std::setw(width) << "Prefch";
  std::cout << std::left << std::setw(width) << "Transform)";
  std::cout << std::left << std::setw(width) << "Transfer";
  std::cout << std::left << std::setw(width) << "Execution";
  std::cout << std::left << std::setw(width) << "Total";
  std::cout << std::endl;

  for (size_t i = 0; i < tunerResults.size(); i++) {
    std::cout << std::left << std::setw(width) << tunerResults.at(i).workGroupSize;
    std::cout << std::left << std::setw(width) << tunerResults.at(i).assignmentsBitSize;
    std::cout << std::left << std::setw(width) << (tunerResults.at(i).assignmentsPrefetch ? "true" : "false");
    std::cout << std::left << std::setw(width) << tunerResults.at(i).clausesBitSize;
    std::cout << std::left << std::setw(width) << (tunerResults.at(i).clausesPrefetch ? "true" : "false");
    std::cout << std::left << std::setw(width) << transformToString(tunerResults.at(i).clausesTransform);
    std::cout << std::left << std::setw(width) << tunerResults.at(i).dataTransferTime;
    std::cout << std::left << std::setw(width) << tunerResults.at(i).executionTime;
    std::cout << std::left << std::setw(width) << (tunerResults.at(i).dataTransferTime + tunerResults.at(i).executionTime);
    std::cout << std::endl;
  }
}

TunerOutput executeRowCol(const size_t M, const int width, std::unique_ptr<CLArray>& clauseDB, 
  std::unique_ptr<PackedArray>& assignmentsArray, std::unique_ptr<PackedArray>& lengthsArray,
  std::vector<int32_t>& target, size_t workGroupSize) {
  assert(clauseDB->numElements() != 0);
  assert(assignmentsArray->numElements() != 0);

  const std::string KERNEL = "../kernels/clause/clause_inspection.cl";

  // Build kernel header
  std::string kernelHeader = "";
  kernelHeader += clauseDB->generateOpenCLCode();
  kernelHeader += assignmentsArray->generateOpenCLCode();
  kernelHeader += lengthsArray->generateOpenCLCode();
  std::cout << "GENERATED OPENCL HEADER" << std::endl;
  std::cout << kernelHeader;
  std::string kernel = appendKernelHeader(KERNEL, kernelHeader);

  // CLTune tuner parameters
  std::vector<int32_t> result(M);
  cltune::Tuner tuner(size_t{PLATFORM_ID}, size_t{DEVICE_ID});
  tuner.AddKernelFromString(kernel, KERNEL_NAME, {M}, {workGroupSize});
  const auto startTime = std::chrono::steady_clock::now();
  tuner.AddArgumentScalar((int)M);
  tuner.AddArgumentInput(clauseDB->getArray());
  tuner.AddArgumentInput(assignmentsArray->getArray());
  tuner.AddArgumentInput(lengthsArray->getArray());
  tuner.AddArgumentOutput(result);
  const auto cpuTime = std::chrono::steady_clock::now() - startTime;
  const auto dataTransferTime = std::chrono::duration<float,std::milli>(cpuTime).count();
  tuner.SetNumRuns(NUMRUNS);
  tuner.Tune();
  tuner.PrintJSON(OUTPUT_JSON_FILE, {});

  // Verify results
  tuner.GetOutput(result);
  verifyOutput(target, result);

  TunerOutput tunerOutput;
  tunerOutput.dataTransferTime = dataTransferTime;
  tunerOutput.executionTime = getExecutionResult(OUTPUT_JSON_FILE);
  tunerOutput.bestExecutionTime = tuner.BestTime();
  return tunerOutput;
}

TunerOutput executeOffset(const size_t M, std::unique_ptr<OffsetArray>& clauseDB, 
  std::unique_ptr<PackedArray>& assignmentsArray, std::vector<int32_t>& target, size_t workGroupSize) {
  assert(clauseDB->numElements() != 0);
  assert(assignmentsArray->numElements() != 0);

  const std::string KERNEL = "../kernels/clause/clause_inspection_offset.cl";

  // Build kernel header
  std::string kernelHeader = "";
  kernelHeader += clauseDB->generateOpenCLCode();
  kernelHeader += assignmentsArray->generateOpenCLCode();
  std::cout << "GENERATED OPENCL HEADER" << std::endl;
  std::cout << kernelHeader;
  std::string kernel = appendKernelHeader(KERNEL, kernelHeader);

  // CLTune tuner parameters
  std::vector<int32_t> result(M);
  cltune::Tuner tuner(size_t{PLATFORM_ID}, size_t{DEVICE_ID});
  tuner.AddKernelFromString(kernel, KERNEL_NAME, {M}, {workGroupSize});
  const auto startTime = std::chrono::steady_clock::now();
  tuner.AddArgumentScalar((int)M);
  tuner.AddArgumentInput(clauseDB->getArray());
  tuner.AddArgumentInput(assignmentsArray->getArray());
  tuner.AddArgumentInput(clauseDB->getOffsets());
  tuner.AddArgumentOutput(result);
  const auto cpuTime = std::chrono::steady_clock::now() - startTime;
  const auto dataTransferTime = std::chrono::duration<float,std::milli>(cpuTime).count();
  tuner.SetNumRuns(NUMRUNS);
  tuner.Tune();
  tuner.PrintJSON(OUTPUT_JSON_FILE, {});

  // Verify results
  tuner.GetOutput(result);
  verifyOutput(target, result);

  TunerOutput tunerOutput;
  tunerOutput.dataTransferTime = dataTransferTime;
  tunerOutput.executionTime = getExecutionResult(OUTPUT_JSON_FILE);
  return tunerOutput;
}

TunerOutput executeMultipage(std::vector<std::vector<int32_t>>& clauses, std::vector<int32_t>& assignments,
  std::unique_ptr<PackedArray>& assignmentsArray, int clausesBitSize, size_t workGroupSize) {
  std::map<int, std::vector<std::vector<int32_t>>> multiPages = getMultiPages(clauses);
  std::map<int, std::unique_ptr<CLArray>> arrMap = transformToMultiPage(multiPages, "clauses", clausesBitSize, false, Transform::ROW_MAJOR, workGroupSize);

  TunerOutput tunerOutput;
  tunerOutput.dataTransferTime = 0.0;
  tunerOutput.executionTime = 0.0;
  for (auto it = arrMap.begin(); it != arrMap.end(); ++it) {
    const int width = it->first;
    std::unique_ptr<CLArray> arr = std::move(it->second);
    std::vector<int32_t> target = clauseInspectionTarget(multiPages[width], assignments);

    assert(arr->numElements() % width == 0); // This should always be a full matrix
    uint32_t M = arr->numElements() / width;

    assert(arr->numElements() != 0);
    assert(assignmentsArray->numElements() != 0);

    const std::string KERNEL = "../kernels/clause/clause_inspection_multi.cl";

    // Build kernel header
    std::string kernelHeader = "";
    kernelHeader += arr->generateOpenCLCode();
    kernelHeader += assignmentsArray->generateOpenCLCode();
    std::cout << "GENERATED OPENCL HEADER" << std::endl;
    std::cout << kernelHeader;
    std::string kernel = appendKernelHeader(KERNEL, kernelHeader);

    // CLTune tuner parameters
    std::vector<int32_t> result(M);
    cltune::Tuner tuner(size_t{PLATFORM_ID}, size_t{DEVICE_ID});
    tuner.AddKernelFromString(kernel, KERNEL_NAME, {M}, {workGroupSize});
    const auto startTime = std::chrono::steady_clock::now();
    tuner.AddArgumentScalar((int)M);
    tuner.AddArgumentScalar(width);
    tuner.AddArgumentInput(arr->getArray());
    tuner.AddArgumentInput(assignmentsArray->getArray());
    tuner.AddArgumentOutput(result);
    const auto cpuTime = std::chrono::steady_clock::now() - startTime;
    const auto dataTransferTime = std::chrono::duration<float,std::milli>(cpuTime).count();
    tuner.SetNumRuns(NUMRUNS);
    tuner.Tune();
    tuner.PrintJSON(OUTPUT_JSON_FILE, {});

    // Verify results
    tuner.GetOutput(result);
    verifyOutput(target, result);

    tunerOutput.dataTransferTime += dataTransferTime;
    tunerOutput.executionTime += tuner.BestTime();
  }
  return tunerOutput;
}

void tuneKernel(std::vector<std::vector<int32_t>>& clauses,
  ArrayConfig2D& clausesConfig,
  std::vector<int32_t>& assignments,
  ArrayConfig1D& assignmentsConfig) {
  const std::vector<size_t> WORKGROUP_SIZES = {32};
  const size_t M = clauses.size();
  std::vector<int32_t> target = clauseInspectionTarget(clauses, assignments);

  std::vector<int32_t> lengths;
  for (int i = 0; i < clauses.size(); i++) {
    lengths.push_back(clauses.at(i).size());
  }

  for (auto workGroupSize : WORKGROUP_SIZES) {
    std::unique_ptr<PackedArray> lengthsArray(new PackedArray("lengths", 32, false, lengths, workGroupSize));

  	for (auto assignmentsBitSize : assignmentsConfig.bitSizes) {
  		for (auto assignmentsPrefetch : assignmentsConfig.prefetches) {
        for (auto clausesBitSize : clausesConfig.bitSizes) {
          for (auto clausesPrefetch : clausesConfig.prefetches) {
  			    for (auto clausesTransform : clausesConfig.transforms) {
              TunerOutput tunerOutput;

              // Create arrays
              std::unique_ptr<PackedArray> assignmentsArray(new PackedArray("assignments", 
                assignmentsBitSize, assignmentsPrefetch, assignments, workGroupSize));

              if (clausesTransform == Transform::ROW_MAJOR) {
                std::unique_ptr<CLArray> clauseDB = std::unique_ptr<RowPaddedArray>(new RowPaddedArray("clauses", clausesBitSize, clausesPrefetch, clauses));
                const int width = static_cast<RowPaddedArray*>(clauseDB.get())->getWidth();
                tunerOutput = executeRowCol(M, width, clauseDB, assignmentsArray, lengthsArray, target, workGroupSize);
              } else if (clausesTransform == Transform::COL_MAJOR) {
                std::unique_ptr<CLArray> clauseDB = std::unique_ptr<ColPaddedArray>(new ColPaddedArray("clauses", clausesBitSize, clausesPrefetch, clauses));
                const int width = static_cast<ColPaddedArray*>(clauseDB.get())->getWidth();
                tunerOutput = executeRowCol(M, width, clauseDB, assignmentsArray, lengthsArray, target, workGroupSize);
              } else if (clausesTransform == Transform::OFFSET) {
                std::unique_ptr<OffsetArray> clauseDB = std::unique_ptr<OffsetArray>(new OffsetArray("clauses", clausesBitSize, clausesPrefetch, clauses, workGroupSize));
                tunerOutput = executeOffset(M, clauseDB, assignmentsArray, target, workGroupSize);
              } else if (clausesTransform == Transform::MULTI_PAGE) {
                tunerOutput = executeMultipage(clauses, assignments, assignmentsArray, clausesBitSize, workGroupSize);
              }

              // Store result
              TunerResult tunerResult;
              tunerResult.workGroupSize = workGroupSize;
              tunerResult.assignmentsBitSize = assignmentsBitSize;
              tunerResult.assignmentsPrefetch = assignmentsPrefetch;
              tunerResult.clausesBitSize = clausesBitSize;
              tunerResult.clausesPrefetch = clausesPrefetch;
              tunerResult.clausesTransform = clausesTransform;
              tunerResult.dataTransferTime = tunerOutput.dataTransferTime;
              tunerResult.executionTime = tunerOutput.executionTime;
              tunerResults.push_back(tunerResult);
            }
          }
  			}
  		}
  	}
  }

  printResults();
}

int main(int argc, char *argv[]) {
	std::vector<std::vector<int32_t>> clauses = readMatrixFromFile(DATA);
  std::vector<int32_t> assignments = readMatrixFromFile(ASSIGNMENT_DATA)[0];

  ArrayConfig2D clausesConfig;
  clausesConfig.bitSizes = {16, 32};
  clausesConfig.prefetches = {false};
  clausesConfig.transforms = {Transform::ROW_MAJOR, Transform::COL_MAJOR, Transform::OFFSET, Transform::MULTI_PAGE};

  ArrayConfig1D assignmentsConfig;
  assignmentsConfig.bitSizes = {32};
  assignmentsConfig.prefetches = {false, true};

  tuneKernel(clauses, clausesConfig, assignments, assignmentsConfig);
}
