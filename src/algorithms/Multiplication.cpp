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

const std::string KERNEL_NAME = "multiplication";
const std::string OUTPUT_JSON_FILE = "bin/tuner_result.json";

struct TunerResult {
  int workGroupSize;
  int m1BitSize;
  bool m1Prefetch;
  Transform m1Transform;
  int m2BitSize;
  bool m2Prefetch;
  Transform m2Transform;
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
    std::cout << std::left << std::setw(width) << tunerResults.at(i).m2BitSize;
    std::cout << std::left << std::setw(width) << (tunerResults.at(i).m2Prefetch ? "true" : "false");
    std::cout << std::left << std::setw(width) << transformToString(tunerResults.at(i).m2Transform);
    std::cout << std::left << std::setw(width) << tunerResults.at(i).dataTransferTime;
    std::cout << std::left << std::setw(width) << tunerResults.at(i).executionTime;
    std::cout << std::left << std::setw(width) << (tunerResults.at(i).dataTransferTime + tunerResults.at(i).executionTime);
    std::cout << std::endl;
  }
}

TunerOutput executeRowCol(const size_t M, const size_t K, const size_t N, std::unique_ptr<CLArray>& m1Array, 
  std::unique_ptr<CLArray>& m2Array, std::vector<int32_t>& target, size_t workGroupSize) {
  assert(m1Array->numElements() != 0);
  assert(m2Array->numElements() != 0);

  const std::string KERNEL = "../kernels/multiplication/multiplication.cl";

  // Build kernel header
  std::string kernelHeader = "";
  kernelHeader += m1Array->generateOpenCLCode();
  kernelHeader += m2Array->generateOpenCLCode();
  std::cout << "GENERATED OPENCL HEADER" << std::endl;
  std::cout << kernelHeader;
  std::string kernel = appendKernelHeader(KERNEL, kernelHeader);

  // CLTune tuner parameters
  std::vector<int32_t> result(M * N);
  cltune::Tuner tuner(size_t{PLATFORM_ID}, size_t{DEVICE_ID});
  tuner.AddKernelFromString(kernel, KERNEL_NAME, {M, N}, {workGroupSize / 2, workGroupSize / 2});
  const auto startTime = std::chrono::steady_clock::now();
  tuner.AddArgumentScalar((int)M);
  tuner.AddArgumentScalar((int)K);
  tuner.AddArgumentScalar((int)N);
  tuner.AddArgumentInput(m1Array->getArray());
  tuner.AddArgumentInput(m2Array->getArray());
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

void tuneKernel(std::vector<size_t> workGroupSizes,
  std::vector<std::vector<int32_t>>& m1, ArrayConfig2D& m1Config,
  std::vector<std::vector<int32_t>>& m2, ArrayConfig2D& m2Config) {
  const size_t M = m1.size();
  const size_t K = m1.at(0).size();
  const size_t N = m2.at(0).size();

  std::vector<int32_t> target = getMultiplicationTarget(m1, m2);

  for (auto workGroupSize : workGroupSizes) {
  	for (auto m1BitSize : m1Config.bitSizes) {
  		for (auto m1Prefetch : m1Config.prefetches) {
        for (auto m1Transform : m1Config.transforms) {
          for (auto m2BitSize : m2Config.bitSizes) {
            for (auto m2Prefetch : m2Config.prefetches) {
    			    for (auto m2Transform : m2Config.transforms) {
                TunerOutput tunerOutput;

                // Build M1 array.
                std::unique_ptr<CLArray> m1Array;
                if (m1Transform == Transform::ROW_MAJOR) {
                  m1Array = std::unique_ptr<RowPaddedArray>(new RowPaddedArray("A", m1BitSize, m1Prefetch, m1));
                } else if (m1Transform == Transform::COL_MAJOR) {
                  m1Array = std::unique_ptr<ColPaddedArray>(new ColPaddedArray("A", m1BitSize, m1Prefetch, m1));
                }

                // Build M2 array.
                std::unique_ptr<CLArray> m2Array;
                if (m2Transform == Transform::ROW_MAJOR) {
                  m2Array = std::unique_ptr<RowPaddedArray>(new RowPaddedArray("B", m2BitSize, m2Prefetch, m2));
                } else if (m2Transform == Transform::COL_MAJOR) {
                  m2Array = std::unique_ptr<ColPaddedArray>(new ColPaddedArray("B", m2BitSize, m2Prefetch, m2));
                } 

                tunerOutput = executeRowCol(M, K, N, m1Array, m2Array, target, workGroupSize);

                // Store result
                TunerResult tunerResult;
                tunerResult.workGroupSize = workGroupSize;
                tunerResult.m1BitSize = m1BitSize;
                tunerResult.m1Prefetch = m1Prefetch;
                tunerResult.m1Transform = m1Transform;
                tunerResult.m2BitSize = m2BitSize;
                tunerResult.m2Prefetch = m2Prefetch;
                tunerResult.m2Transform = m2Transform;
                tunerResult.dataTransferTime = tunerOutput.dataTransferTime;
                tunerResult.executionTime = tunerOutput.executionTime;
                tunerResults.push_back(tunerResult);
              }
            }
    			}
        }
  		}
  	}
  }

  printResults();
}

int main(int argc, char *argv[]) {
  const std::vector<size_t> WORKGROUP_SIZES = {32};

	std::vector<std::vector<int32_t>> m1 = initMatrix(1024, 512, 1000);
  std::vector<std::vector<int32_t>> m2 = initMatrix(512, 2048, 1000);

  ArrayConfig2D m1Config;
  m1Config.bitSizes = {32};
  m1Config.prefetches = {false};
  m1Config.transforms = {Transform::ROW_MAJOR, Transform::COL_MAJOR};

  ArrayConfig2D m2Config;
  m2Config.bitSizes = {32};
  m2Config.prefetches = {false};
  m2Config.transforms = {Transform::ROW_MAJOR, Transform::COL_MAJOR};

  tuneKernel(WORKGROUP_SIZES, m1, m1Config, m2, m2Config);
}
