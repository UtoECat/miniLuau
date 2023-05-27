TARGET_TEST=./luau-test

CXXFLAGS =  -Wall -Wsign-compare -Wno-unused -I./ -I../
CXXFLAGS += -O0 -g -Wextra#-DNDEBUG -O2 #  
CXXFLAGS += -fno-math-errno -I.
UNIFLAGS  = -fsanitize=address -fsanitize=undefined -DLUAU_ENABLE_ASSERT

SRCS := ./luau.o ./baseio.cpp ./test.cpp ./linenoise.o

all : $(TARGET_TEST)

./luau.o : ../luau.cpp
	g++ $(CXXFLAGS) $(UNIFLAGS) -c $^ -o $@ -std=c++17

./linenoise.o: ./linenoise.c
	gcc -c $^ -o $@

$(TARGET_TEST) : $(SRCS)
	g++ $(CXXFLAGS) $(UNIFLAGS) $^ -o $@ -std=c++17

clear:
	rm ./luau.o ./linenoise.o $(TARGET_TEST)
