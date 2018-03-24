#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define MAX_BITSIZE 32

#if defined(__APPLE__) || defined(__MACOSX)
#define PLATFORM_ID 0
#define DEVICE_ID 1
#else
#define PLATFORM_ID 0
#define DEVICE_ID 0
#endif

#define BITSIZES {32, 2}
#define GRPSIZES {1, 2, 4, 8, 16, 32}
#define NUMRUNS 20

#endif
