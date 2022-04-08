BASE=.
SDSL_INCLUDE=${BASE}/include
SDSL_LIB=${BASE}/lib

all: count-test suffix-array

count-test:	index.cpp count-test.cpp index.h common.h
	g++ index.cpp count-test.cpp -I ${SDSL_INCLUDE} -L ${SDSL_LIB} -lsdsl -O3 -DNDEBUG -o $@

suffix-array:	suffix-array.cpp common.h
	g++ suffix-array.cpp -O3 -DNDEBUG -o $@
