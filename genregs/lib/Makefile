SRC_DIR = ../src
SOURCES = $(wildcard ${SRC_DIR}/*.cpp)
LIBFILE = libhbm_driver.so

all: ${LIBFILE}

${LIBFILE}: ${SOURCES}
	g++ $^ -I${SRC_DIR} -fPIC -shared -o $@

