TARGET_TEST=./luau-test
TARGET_COMPILER=./luauc

CXXFLAGS =  -Wall -Wsign-compare -Wno-unused -I./ -I../
DEBUG=1

ifeq ($(MODE),1)
CXXFLAGS += -O0 -g -Wextra  -DLUAU_ENABLE_ASSERT
UNIFLAGS  = -fsanitize=address -fsanitize=undefined
else
CXXFLAGS += -DNDEBUG -O2 -g0 
CXXFLAGS += -fno-math-errno -I.
UNIFLAGS  = -s # strip debug
endif

SRCS := ./luau.o ./baseio.cpp ./test.cpp ./linenoise.o

all : $(TARGET_TEST) $(TARGET_COMPILER)
CC ?= gcc
CXX ?= g++

./luau.o : ../luau.cpp
	$(CXX) $(CXXFLAGS) $(UNIFLAGS) -c $^ -o $@ -std=c++17

./linenoise.o: ./linenoise.c
	$(CC) -c $^ -o $@ $(CXXFLAGS) $(UNIFLAGS)

$(TARGET_TEST) : $(SRCS)
	$(CXX) $(CXXFLAGS) $(UNIFLAGS) $^ -o $@ -std=c++17

$(TARGET_COMPILER) : ./luau.o ./compiler.cpp ./baseio.cpp
	$(CXX) $(CXXFLAGS) $(UNIFLAGS) $^ -o $@ -std=c++17

clear:
	rm ./luau.o ./linenoise.o $(TARGET_TEST) $(TARGET_COMPILER)
