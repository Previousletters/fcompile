GXX = g++

MOD = $(wildcard mod/*.c)
LIB = $(wildcard lib/*.cpp)
INC = include

all: exe

exe: main.c $(MOD) $(LIB)
	$(GXX) $^ -I$(INC) -o $@