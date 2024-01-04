DIR := .
SRCs := $(wildcard ${DIR}/src/*.cc)
INCs := ${DIR}/include
MAIN := ${DIR}/test/main.cc
EXE := ${DIR}/exe

all: ${EXE}

${EXE}: ${MAIN} ${SRCs}
	g++ $^ -I${INCs} -o $@


.PHONY: all
