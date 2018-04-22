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

#define ROW_MAJOR 0
#define COL_MAJOR 1
#define OFFSET 2
#define MULTI_PAGE 3

struct ArrayConfig1D {
  std::vector<int> bitSizes;
  std::vector<bool> prefetches;
};

struct ArrayConfig2D {
	std::vector<int> bitSizes;
	std::vector<bool> prefetches;
	std::vector<int> transforms;
};

#define NUMRUNS 20

struct TunerResult {
  int workGroupSize;
  int bitSize;
  bool prefetch;
  std::string transform;
  double executionTime;
  double dataTransferTime;
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
