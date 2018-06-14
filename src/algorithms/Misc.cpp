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

const std::string KERNEL_NAME = "misc";
const std::string OUTPUT_JSON_FILE = "bin/tuner_result.json";

struct TunerResult {
  int workGroupSize;
  int m1BitSize;
  bool m1Prefetch;
  Transform m1Transform;
  double executionTime;
  double dataTransferTime;
};

std::vector<TunerResult> tunerResults;

void printResults() {
  const int width = 12;

  std::cout << "Summary:" << std::endl;
  std::cout << std::left << std::setw(width) << "GrpSize";
  std::cout << std::left << std::setw(width) << "(Bitsize";
  std::cout << std::left << std::setw(width) << "Prefch";
  std::cout << std::left << std::setw(width) << "Transform)";
  std::cout << std::left << std::setw(width) << "Transfer";
  std::cout << std::left << std::setw(width) << "Execution";
  std::cout << std::left << std::setw(width) << "Total";
  std::cout << std::endl;

  for (size_t i = 0; i < tunerResults.size(); i++) {
    std::cout << std::left << std::setw(width) << tunerResults.at(i).workGroupSize;
    std::cout << std::left << std::setw(width) << tunerResults.at(i).m1BitSize;
    std::cout << std::left << std::setw(width) << (tunerResults.at(i).m1Prefetch ? "true" : "false");
    std::cout << std::left << std::setw(width) << transformToString(tunerResults.at(i).m1Transform);
    std::cout << std::left << std::setw(width) << tunerResults.at(i).dataTransferTime;
    std::cout << std::left << std::setw(width) << tunerResults.at(i).executionTime;
    std::cout << std::left << std::setw(width) << (tunerResults.at(i).dataTransferTime + tunerResults.at(i).executionTime);
    std::cout << std::endl;
  }
}

TunerOutput executePacking(const size_t M, std::unique_ptr<CLArray>& m1Array, size_t workGroupSize) {
  assert(m1Array->numElements() != 0);

  const std::string KERNEL = "../kernels/misc/packing.cl";

  // Build kernel header
  std::string kernelHeader = "";
  kernelHeader += m1Array->generateOpenCLCode();
  // std::cout << "GENERATED OPENCL HEADER" << std::endl;
  // std::cout << kernelHeader;
  std::string kernel = appendKernelHeader(KERNEL, kernelHeader);

  // CLTune tuner parameters
  size_t globalWorkItems = 100000000;
  cltune::Tuner tuner(size_t{PLATFORM_ID}, size_t{DEVICE_ID});
  tuner.AddKernelFromString(kernel, KERNEL_NAME, {globalWorkItems}, {workGroupSize});
  const auto startTime = std::chrono::steady_clock::now();
  tuner.AddArgumentScalar((int)globalWorkItems);
  tuner.AddArgumentScalar((int)m1Array->numElements());
  tuner.AddArgumentInput(m1Array->getArray());
  const auto cpuTime = std::chrono::steady_clock::now() - startTime;
  const auto dataTransferTime = std::chrono::duration<float,std::milli>(cpuTime).count();
  tuner.SetNumRuns(NUMRUNS);
  tuner.Tune();
  tuner.PrintJSON(OUTPUT_JSON_FILE, {});

  TunerOutput tunerOutput;
  tunerOutput.dataTransferTime = dataTransferTime;
  tunerOutput.executionTime = getExecutionResult(OUTPUT_JSON_FILE);
  tunerOutput.bestExecutionTime = tuner.BestTime();
  return tunerOutput;
}

TunerOutput executePrefetching(std::unique_ptr<CLArray>& m1Array, size_t workGroupSize) {
  assert(m1Array->numElements() != 0);

  const std::string KERNEL = "../kernels/misc/prefetching.cl";

  // Build kernel header
  std::string kernelHeader = "";
  kernelHeader += m1Array->generateOpenCLCode();
  //std::cout << "GENERATED OPENCL HEADER" << std::endl;
  //std::cout << kernelHeader;
  std::string kernel = appendKernelHeader(KERNEL, kernelHeader);

  // CLTune tuner parameters
  size_t globalWorkItems = 100000000;
  std::vector<int32_t> result(globalWorkItems);
  cltune::Tuner tuner(size_t{PLATFORM_ID}, size_t{DEVICE_ID});
  tuner.AddKernelFromString(kernel, KERNEL_NAME, {globalWorkItems}, {workGroupSize});
  const auto startTime = std::chrono::steady_clock::now();
  tuner.AddArgumentScalar((int)globalWorkItems);
  tuner.AddArgumentScalar((int)m1Array->numElements());
  tuner.AddArgumentInput(m1Array->getArray());
  tuner.AddArgumentOutput(result);
  const auto cpuTime = std::chrono::steady_clock::now() - startTime;
  const auto dataTransferTime = std::chrono::duration<float,std::milli>(cpuTime).count();
  tuner.SetNumRuns(NUMRUNS);
  tuner.Tune();
  tuner.PrintJSON(OUTPUT_JSON_FILE, {});

  TunerOutput tunerOutput;
  tunerOutput.dataTransferTime = dataTransferTime;
  tunerOutput.executionTime = getExecutionResult(OUTPUT_JSON_FILE);
  tunerOutput.bestExecutionTime = tuner.BestTime();
  return tunerOutput;
}

TunerOutput executeRowCol(const size_t M, const size_t width, std::unique_ptr<CLArray>& m1Array, size_t workGroupSize) {
  assert(m1Array->numElements() != 0);

  const std::string KERNEL = "../kernels/misc/rowcol.cl";

  // Build kernel header
  std::string kernelHeader = "";
  kernelHeader += m1Array->generateOpenCLCode();
  // std::cout << "GENERATED OPENCL HEADER" << std::endl;
  // std::cout << kernelHeader;
  std::string kernel = appendKernelHeader(KERNEL, kernelHeader);

  // CLTune tuner parameters
  std::vector<int32_t> result(M * width);
  cltune::Tuner tuner(size_t{PLATFORM_ID}, size_t{DEVICE_ID});
  tuner.AddKernelFromString(kernel, KERNEL_NAME, {M}, {workGroupSize});
  const auto startTime = std::chrono::steady_clock::now();
  tuner.AddArgumentScalar((int)M);
  tuner.AddArgumentScalar((int)width);
  tuner.AddArgumentInput(m1Array->getArray());
  tuner.AddArgumentOutput(result);
  const auto cpuTime = std::chrono::steady_clock::now() - startTime;
  const auto dataTransferTime = std::chrono::duration<float,std::milli>(cpuTime).count();
  tuner.SetNumRuns(NUMRUNS);
  tuner.Tune();
  tuner.PrintJSON(OUTPUT_JSON_FILE, {});

  TunerOutput tunerOutput;
  tunerOutput.dataTransferTime = dataTransferTime;
  tunerOutput.executionTime = getExecutionResult(OUTPUT_JSON_FILE);
  tunerOutput.bestExecutionTime = tuner.BestTime();
  return tunerOutput;
}

void packing(std::vector<size_t> workGroupSizes,
  std::vector<std::vector<int32_t>>& m1, ArrayConfig2D& m1Config) {
  const size_t M = m1.size();

  for (auto workGroupSize : workGroupSizes) {
  	for (auto m1BitSize : m1Config.bitSizes) {
  		for (auto m1Prefetch : m1Config.prefetches) {
        for (auto m1Transform : m1Config.transforms) {
          TunerOutput tunerOutput;
          std::unique_ptr<CLArray> m1Array;
          if (m1Transform == Transform::ROW_MAJOR) {
            m1Array = std::unique_ptr<RowPaddedArray>(new RowPaddedArray("A", m1BitSize, m1Prefetch, m1));
          } else if (m1Transform == Transform::COL_MAJOR) {
            m1Array = std::unique_ptr<ColPaddedArray>(new ColPaddedArray("A", m1BitSize, m1Prefetch, m1));
          }

          tunerOutput = executePacking(M, m1Array, workGroupSize);

          // Store result
          TunerResult tunerResult;
          tunerResult.workGroupSize = workGroupSize;
          tunerResult.m1BitSize = m1BitSize;
          tunerResult.m1Prefetch = m1Prefetch;
          tunerResult.m1Transform = m1Transform;
          tunerResult.dataTransferTime = tunerOutput.dataTransferTime;
          tunerResult.executionTime = tunerOutput.executionTime;
          tunerResults.push_back(tunerResult);
        }
  		}
  	}
  }

  printResults();
}

void prefetching(std::vector<size_t> workGroupSizes,
  std::vector<std::vector<int32_t>>& m1, ArrayConfig2D& m1Config) {

  for (auto workGroupSize : workGroupSizes) {
    for (auto m1BitSize : m1Config.bitSizes) {
      for (auto m1Prefetch : m1Config.prefetches) {
        for (auto m1Transform : m1Config.transforms) {
          TunerOutput tunerOutput;

          std::unique_ptr<CLArray> m1Array;
          if (m1Transform == Transform::ROW_MAJOR) {
            m1Array = std::unique_ptr<RowPaddedArray>(new RowPaddedArray("A", m1BitSize, m1Prefetch, m1, workGroupSize));
          } else if (m1Transform == Transform::COL_MAJOR) {
            m1Array = std::unique_ptr<ColPaddedArray>(new ColPaddedArray("A", m1BitSize, m1Prefetch, m1, workGroupSize));
          }

          tunerOutput = executePrefetching(m1Array, workGroupSize);

          // Store result
          TunerResult tunerResult;
          tunerResult.workGroupSize = workGroupSize;
          tunerResult.m1BitSize = m1BitSize;
          tunerResult.m1Prefetch = m1Prefetch;
          tunerResult.m1Transform = m1Transform;
          tunerResult.dataTransferTime = tunerOutput.dataTransferTime;
          tunerResult.executionTime = tunerOutput.executionTime;
          tunerResults.push_back(tunerResult);
        }
      }
    }
  }

  printResults();
}

void rowcol(std::vector<size_t> workGroupSizes,
  std::vector<std::vector<int32_t>>& m1, ArrayConfig2D& m1Config) {
  const size_t M = m1.size();
  const size_t N = m1.at(0).size();

  for (auto workGroupSize : workGroupSizes) {
    for (auto m1BitSize : m1Config.bitSizes) {
      for (auto m1Prefetch : m1Config.prefetches) {
        for (auto m1Transform : m1Config.transforms) {
          TunerOutput tunerOutput;

          std::unique_ptr<CLArray> m1Array;
          if (m1Transform == Transform::ROW_MAJOR) {
            m1Array = std::unique_ptr<RowPaddedArray>(new RowPaddedArray("A", m1BitSize, m1Prefetch, m1));
          } else if (m1Transform == Transform::COL_MAJOR) {
            m1Array = std::unique_ptr<ColPaddedArray>(new ColPaddedArray("A", m1BitSize, m1Prefetch, m1));
          }

          tunerOutput = executeRowCol(M, N, m1Array, workGroupSize);

          // Store result
          TunerResult tunerResult;
          tunerResult.workGroupSize = workGroupSize;
          tunerResult.m1BitSize = m1BitSize;
          tunerResult.m1Prefetch = m1Prefetch;
          tunerResult.m1Transform = m1Transform;
          tunerResult.dataTransferTime = tunerOutput.dataTransferTime;
          tunerResult.executionTime = tunerOutput.executionTime;
          tunerResults.push_back(tunerResult);
        }
      }
    }
  }

  printResults();
}

int main(int argc, char *argv[]) {
  const std::vector<size_t> WORKGROUP_SIZES = {256};

  std::vector<std::vector<int32_t>> m1 = initMatrix(1, 100000000, 4);
  ArrayConfig2D m1Config;
  m1Config.bitSizes = {2, 32};
  m1Config.prefetches = {false};
  m1Config.transforms = {Transform::ROW_MAJOR};
  packing(WORKGROUP_SIZES, m1, m1Config);

  std::vector<std::vector<int32_t>> m2 = initMatrix(1, 32, 1000);
  ArrayConfig2D m2Config;
  m2Config.bitSizes = {32};
  m2Config.prefetches = {false, true};
  m2Config.transforms = {Transform::ROW_MAJOR};
  prefetching(WORKGROUP_SIZES, m2, m2Config);

  std::vector<std::vector<int32_t>> m3 = initMatrix(4096, 4096, 1000);
  ArrayConfig2D m3Config;
  m3Config.bitSizes = {32};
  m3Config.prefetches = {false};
  m3Config.transforms = {Transform::ROW_MAJOR, Transform::COL_MAJOR};
  rowcol(WORKGROUP_SIZES, m3, m3Config);
}
