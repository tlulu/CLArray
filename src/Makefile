OS = $(shell uname)

CXX = g++
CXXFLAGS = -Wall -std=c++11

OBJ_DIR = bin

COMMON_OBJECTS = \
	common/MathUtils.o \
	common/IOUtils.o

LIB_OBJECTS = \
	lib/json11.o

TRANSFORM_OBJECTS = \
	transforms/CLArray.o \
	transforms/PackedArray.o \
	transforms/RowPaddedArray.o \
	transforms/ColPaddedArray.o \
	transforms/OffsetArray.o

ALGORITHM_OBJECTS = \
	algorithms/ClauseInspection.o \
	algorithms/Hadamard.o

TEST_OBJECTS = \
	tests/RunTests.o \
	tests/PackedArrayTest.o \
	tests/RowPaddedArrayTest.o \
	tests/ColPaddedArrayTest.o \
	tests/OffsetArrayTest.o \
	tests/MultipageTest.o

CLAUSE_EXEC = $(OBJ_DIR)/clause
HADAMARD_EXEC = $(OBJ_DIR)/hadamard
TEST_EXEC = $(OBJ_DIR)/unitTests
DEVICE_INFO_EXEC = $(OBJ_DIR)/deviceInfo

EXECS = ${CLAUSE_EXEC} ${HADAMARD_EXEC} ${TEST_EXEC} ${DEVICE_INFO_EXEC}
OBJECTS = \
	${COMMON_OBJECTS} \
	${LIB_OBJECTS} \
	${TRANSFORM_OBJECTS} \
	${ALGORITHM_OBJECTS} \
	${TEST_OBJECTS}
DEPENDS = ${OBJECTS:.o=.d}

.PHONY: all clean

all: $(OBJ_DIR) ${EXECS}

clean:
	rm -f ${OBJECTS} ${EXECS} ${DEPENDS}
	rm -rf $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

${CLAUSE_EXEC}: ${COMMON_OBJECTS} ${TRANSFORM_OBJECTS} algorithms/ClauseInspection.o ${LIB_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@ -lcltune

${HADAMARD_EXEC}: ${COMMON_OBJECTS} ${TRANSFORM_OBJECTS} algorithms/Hadamard.o ${LIB_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@ -lcltune

${TEST_EXEC}: ${COMMON_OBJECTS} ${TRANSFORM_OBJECTS} ${TEST_OBJECTS} ${LIB_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@ -lgtest

${DEVICE_INFO_EXEC}:
ifeq ($(OS), Darwin)
	${CXX} ${CXXFLAGS} DeviceInfo.cpp $^ -o $@ -framework OpenCL
else
	${CXX} ${CXXFLAGS} DeviceInfo.cpp $^ -o $@ -lOpenCL
endif

-include ${DEPENDS}
