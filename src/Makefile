CXX = g++
CXXFLAGS = -Wall -std=c++11

OBJ_DIR = bin

COMMON_OBJECTS = \
	common/MathUtils.o \
	common/IOUtils.o

TRANSFORM_OBJECTS = \
	transforms/BaseArray.o \
	transforms/PackedArray.o \
	transforms/RowPaddedArray.o

ALGORITHM_OBJECTS = \
	algorithms/ClauseInspection.o

TEST_OBJECTS = \
	tests/RunTests.o \
	tests/PackedArrayTest.o \
	tests/RowPaddedArrayTest.o

CLAUSE_EXEC = $(OBJ_DIR)/clause
TEST_EXEC = $(OBJ_DIR)/unitTests

EXECS = ${CLAUSE_EXEC} ${TEST_EXEC}
OBJECTS = \
	${COMMON_OBJECTS} \
	${TRANSFORM_OBJECTS} \
	${ALGORITHM_OBJECTS} \
	${TEST_OBJECTS}
DEPENDS = ${OBJECTS:.o=.d}

.PHONY: all clean

all: $(OBJ_DIR) ${EXECS}

clean:
	rm -f ${OBJECTS} ${EXECS} ${DEPENDS}

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

${CLAUSE_EXEC}: ${COMMON_OBJECTS} ${TRANSFORM_OBJECTS} ${ALGORITHM_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@ -lcltune

${TEST_EXEC}: ${COMMON_OBJECTS} ${TRANSFORM_OBJECTS} ${TEST_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@ -lgtest

-include ${DEPENDS}
