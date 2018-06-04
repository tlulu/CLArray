#include "../../includes/CLArray.h"
#include "../../includes/PackedArray.h"
#include "../../includes/ColPaddedArray.h"
#include "../../includes/RowPaddedArray.h"
#include "../../includes/OffsetArray.h"
#include "../../includes/MathUtils.h"
#include "../../includes/IOUtils.h"
#include "../../includes/globals.h"
#include "../../includes/cltune.h"

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>

const std::string KERNEL_NAME = "hadamard";
const std::string DATA = "../data/hadamard/small_hada_bounded.test";
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

TunerOutput executeRowCol(const size_t M, const size_t width, std::unique_ptr<CLArray>& m1Array, 
  std::unique_ptr<CLArray>& m2Array, std::vector<int32_t>& target, size_t workGroupSize) {
  assert(m1Array->numElements() != 0);
  assert(m2Array->numElements() != 0);

  const std::string KERNEL = "../kernels/hadamard/hadamard.cl";

  // Build kernel header
  std::string kernelHeader = "";
  kernelHeader += m1Array->generateOpenCLCode();
  kernelHeader += m2Array->generateOpenCLCode();
  std::cout << "GENERATED OPENCL HEADER" << std::endl;
  std::cout << kernelHeader;
  std::string kernel = appendKernelHeader(KERNEL, kernelHeader);

  // CLTune tuner parameters
  std::vector<int32_t> result(M * width);
  cltune::Tuner tuner(size_t{PLATFORM_ID}, size_t{DEVICE_ID});
  tuner.AddKernelFromString(kernel, KERNEL_NAME, {M, width}, {workGroupSize / 2, workGroupSize / 2});
  const auto startTime = std::chrono::steady_clock::now();
  tuner.AddArgumentScalar((int)M);
  tuner.AddArgumentScalar(width);
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

TunerOutput executeOffset(const size_t M, const size_t maxWidth, const size_t totalElements,
  std::unique_ptr<OffsetArray>& m1Array, std::unique_ptr<CLArray>& m2Array, 
  std::vector<int32_t>& target, size_t workGroupSize) {
  assert(m1Array->numElements() != 0);
  assert(m2Array->numElements() != 0);

  const std::string KERNEL = "../kernels/hadamard/hadamard_offset.cl";

  // Build kernel header
  std::string kernelHeader = "";
  kernelHeader += m1Array->generateOpenCLCode();
  kernelHeader += m2Array->generateOpenCLCode();
  std::cout << "GENERATED OPENCL HEADER" << std::endl;
  std::cout << kernelHeader;
  std::string kernel = appendKernelHeader(KERNEL, kernelHeader);

  // CLTune tuner parameters
  std::vector<int32_t> result(totalElements);
  cltune::Tuner tuner(size_t{PLATFORM_ID}, size_t{DEVICE_ID});
  tuner.AddKernelFromString(kernel, KERNEL_NAME, {M, maxWidth}, {workGroupSize / 2, workGroupSize / 2});
  const auto startTime = std::chrono::steady_clock::now();
  tuner.AddArgumentScalar((int)M);
  tuner.AddArgumentInput(m1Array->getArray());
  tuner.AddArgumentInput(m2Array->getArray());
  tuner.AddArgumentInput(m1Array->getOffsets());
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

TunerOutput executeMultipage(std::vector<std::vector<int32_t>>& m1, std::vector<std::vector<int32_t>>& m2,
  int m1BitSize, int m2BitSize, size_t workGroupSize) {
  std::map<int, std::vector<std::vector<int32_t>>> multiPagesM1 = getMultiPages(m1);
  std::map<int, std::vector<std::vector<int32_t>>> multiPagesM2 = getMultiPages(m2);
  std::map<int, std::unique_ptr<CLArray>> arrMapM1 = transformToMultiPage(multiPagesM1, "A", m1BitSize, false, Transform::ROW_MAJOR, workGroupSize);
  std::map<int, std::unique_ptr<CLArray>> arrMapM2 = transformToMultiPage(multiPagesM2, "B", m2BitSize, false, Transform::ROW_MAJOR, workGroupSize);
  
  TunerOutput tunerOutput;
  tunerOutput.dataTransferTime = 0.0;
  tunerOutput.executionTime = 0.0;
  for (auto it = arrMapM1.begin(); it != arrMapM1.end(); ++it) {
    const int width = it->first;
    std::unique_ptr<CLArray> m1Array = std::move(it->second);
    std::unique_ptr<CLArray> m2Array = std::move(arrMapM2[width]);
    std::vector<int32_t> target = hadamardTarget(multiPagesM1[width], multiPagesM2[width]);

    assert(m1Array->numElements() % width == 0); // This should always be a full matrix
    uint32_t M = m1Array->numElements() / width;

    TunerOutput output = executeRowCol(M, width, m1Array, m2Array, target, workGroupSize);
    tunerOutput.dataTransferTime += output.dataTransferTime;
    tunerOutput.executionTime += output.bestExecutionTime;
  }
  return tunerOutput;
}

void tuneKernel(std::vector<std::vector<int32_t>>& m1, ArrayConfig2D& m1Config,
  std::vector<std::vector<int32_t>>& m2, ArrayConfig2D& m2Config) {
  const std::vector<size_t> WORKGROUP_SIZES = {32};
  const size_t M = m1.size();

  for (auto workGroupSize : WORKGROUP_SIZES) {
  	for (auto m1BitSize : m1Config.bitSizes) {
  		for (auto m1Prefetch : m1Config.prefetches) {
        for (auto m1Transform : m1Config.transforms) {
          for (auto m2BitSize : m2Config.bitSizes) {
            for (auto m2Prefetch : m2Config.prefetches) {
    			    for (auto m2Transform : m2Config.transforms) {
                TunerOutput tunerOutput;

                // If only one of the arrays are multipage.
                if ((m1Transform == Transform::MULTI_PAGE && m2Transform != Transform::MULTI_PAGE) ||
                    (m1Transform != Transform::MULTI_PAGE && m2Transform == Transform::MULTI_PAGE)) {
                  continue;
                }

                // Execute multipage if both arrays are multipage
                if (m1Transform == Transform::MULTI_PAGE && m2Transform == Transform::MULTI_PAGE) {
                  tunerOutput = executeMultipage(m1, m2, m1BitSize, m2BitSize, workGroupSize);
                } else {
                  // Build M2 array.
                  std::unique_ptr<CLArray> m2Array;
                  if (m2Transform == Transform::ROW_MAJOR) {
                    m2Array = std::unique_ptr<RowPaddedArray>(new RowPaddedArray("B", m2BitSize, m2Prefetch, m2));
                  } else if (m2Transform == Transform::COL_MAJOR) {
                    m2Array = std::unique_ptr<ColPaddedArray>(new ColPaddedArray("B", m2BitSize, m2Prefetch, m2));
                  } 

                  // Build M1 array.
                  if (m1Transform == Transform::ROW_MAJOR) {
                    std::unique_ptr<CLArray> m1Array = std::unique_ptr<RowPaddedArray>(new RowPaddedArray("A", m1BitSize, m1Prefetch, m1));
                    const int width = static_cast<RowPaddedArray*>(m1Array.get())->getWidth();
                    std::vector<std::vector<int32_t>> paddedM1 = m1;
                    std::vector<std::vector<int32_t>> paddedM2 = m2;
                    addPadding(&paddedM1);
                    addPadding(&paddedM2);
                    std::vector<int32_t> target = hadamardTarget(paddedM1, paddedM2);
                    tunerOutput = executeRowCol(M, width, m1Array, m2Array, target, workGroupSize);
                  } else if (m1Transform == Transform::COL_MAJOR) {
                    std::unique_ptr<CLArray> m1Array = std::unique_ptr<ColPaddedArray>(new ColPaddedArray("A", m1BitSize, m1Prefetch, m1));
                    const int width = static_cast<ColPaddedArray*>(m1Array.get())->getWidth();
                    std::vector<std::vector<int32_t>> paddedM1 = m1;
                    std::vector<std::vector<int32_t>> paddedM2 = m2;
                    addPadding(&paddedM1);
                    addPadding(&paddedM2);
                    std::vector<int32_t> target = hadamardTarget(paddedM1, paddedM2);
                    tunerOutput = executeRowCol(M, width, m1Array, m2Array, target, workGroupSize);
                  } else if (m1Transform == Transform::OFFSET) {
                    std::unique_ptr<OffsetArray> m1Array = std::unique_ptr<OffsetArray>(new OffsetArray("A", m1BitSize, m1Prefetch, m1, workGroupSize));
                    std::vector<int32_t> target = hadamardTarget(m1, m2);
                    int maxWidth = getMaxWidth(m1);
                    int totalElements = getNumElements(m1);
                    tunerOutput = executeOffset(M, maxWidth, totalElements, m1Array, m2Array, target, workGroupSize);
                  }
                }

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
	std::vector<std::vector<int32_t>> m1 = readMatrixFromFile(DATA);
  std::vector<std::vector<int32_t>> m2 = m1;

  ArrayConfig2D m1Config;
  m1Config.bitSizes = {32}; // DO NOT BITPACK NEGATIVE VALUES!!
  m1Config.prefetches = {false};
  m1Config.transforms = {Transform::ROW_MAJOR, Transform::COL_MAJOR, Transform::OFFSET, Transform::MULTI_PAGE};

  ArrayConfig2D m2Config;
  m2Config.bitSizes = {32};
  m2Config.prefetches = {false};
  m2Config.transforms = {Transform::ROW_MAJOR, Transform::COL_MAJOR, Transform::MULTI_PAGE};

  tuneKernel(m1, m1Config, m2, m2Config);
}
