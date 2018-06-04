#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <string>
#include <vector>

// OpenCL constants
#if defined(__APPLE__) || defined(__MACOSX)
#define PLATFORM_ID 0
#define DEVICE_ID 1
#else
#define PLATFORM_ID 0
#define DEVICE_ID 0
#endif

// Tuner constants
#define MAX_BITSIZE 32
#define NUMRUNS 20

enum Transform {
  ROW_MAJOR = 1,
  COL_MAJOR = 2,
  OFFSET = 3,
  MULTI_PAGE = 4
};

struct ArrayConfig1D {
  std::vector<int> bitSizes;
  std::vector<bool> prefetches;
};

struct ArrayConfig2D {
	std::vector<int> bitSizes;
	std::vector<bool> prefetches;
	std::vector<Transform> transforms;
};

struct TunerOutput {
  double dataTransferTime;
  double executionTime;
  double bestExecutionTime;
};

// Clause inspection constants
enum Assignment {
  TRUE = 1,
  FALSE = 0,
  UNDEF = 2
};

enum Result {
  SAT = 1,
  CONFLICT = 2,
  UNIT = 3,
  WASTE = 4,
  UNRES = 5
};

#endif
