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
	transforms/ColPaddedArray.o

ALGORITHM_OBJECTS = \
	algorithms/ClauseInspection.o

TEST_OBJECTS = \
	tests/RunTests.o \
	tests/PackedArrayTest.o \
	tests/RowPaddedArrayTest.o \
	tests/ColPaddedArrayTest.o

CLAUSE_EXEC = $(OBJ_DIR)/clause
TEST_EXEC = $(OBJ_DIR)/unitTests

EXECS = ${CLAUSE_EXEC} ${TEST_EXEC}
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
	rm -r $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

${CLAUSE_EXEC}: ${COMMON_OBJECTS} ${TRANSFORM_OBJECTS} ${ALGORITHM_OBJECTS} ${LIB_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@ -lcltune

${TEST_EXEC}: ${COMMON_OBJECTS} ${TRANSFORM_OBJECTS} ${TEST_OBJECTS} ${LIB_OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@ -lgtest

-include ${DEPENDS}
