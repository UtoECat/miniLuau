TARGET_CMP =./build/libLuau.Compiler.a
TARGET_VM  =./build/libLuau.VM.a
TARGET_TEST=./build/luau-test

AR=ar qc
RANLIB=ranlib
MKDIR = mkdir -p

# select how many parrallel task you want :p
MAKEFLAGS+=-r -j4

CXX=g++
CXXFLAGS = -O2 -DNDEBUG -Wall -Wsign-compare -Wno-unused -I./include -I./src
CXXFLAGS += -fno-math-errno -std=c++17 -I.
UNIFLAGS =

SRCS_VM := ./src/*.cpp
OBJS_VM := $(SRCS_VM:%=./build/%.o)
DEPS = $(OBJS_VM:.o=.d)

SRCS_CMP:= ./src/Luau/*.cpp
OBJS_CMP:= $(SRCS_CMP:%=./build/%.o)
DEPS += $(OBJS_CMP:.o=.d)

#Suppress display of executed commands.
$(VERBOSE).SILENT:

all:
	@echo "[MAKE] INFO : Mini Luau build system is started."
	@echo "[MAKE] INFO : CXX = $(CXX); PLATFORM = $(OS)."
	@echo "[MAKE] INFO : Avaliable targets Luau.Test $(TARGET_VM) $(TARGET_CMP)"
	make $(TARGET_CMP) $(TARGET_VM)

Luau.Test: $(TARGET_TEST)

$(TARGET_TEST): test.cpp $(TARGET_CMP) $(TARGET_VM)
	echo "[MAKE] CXX : $^ => $@"
	$(CXX) $(CXXFLAGS) $(UNIFLAGS) $^ $(TARGET) -o $@
	echo "[MAKE] SUCCESS : Luau.Test binary build is done!"

./build/%.cpp.o: %.cpp
	echo "[MAKE] CXX $< => $@"
	$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) $(UNIFLAGS) -c $< -o $@

$(TARGET_CMP): $(OBJS_CMP)
	$(AR) $@ $^
	echo "[MAKE] AR : $< => $@"
	$(RANLIB) $@
	echo "[MAKE] RANLIB : $@"
	echo "[MAKE] SUCCESS : Luau.Compiler library build is done!"

$(TARGET_VM): $(OBJS_VM)
	$(AR) $@ $^
	echo "[MAKE] AR : $< => $@"
	$(RANLIB) $@
	echo "[MAKE] RANLIB : $@"
	echo "[MAKE] SUCCESS : Luau.VM library build is done!"


.PHONY: all Luau.Test clean

-include $(DEPS)

clean:
	rm -rf ./build
