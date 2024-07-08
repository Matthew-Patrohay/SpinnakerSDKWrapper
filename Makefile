# Makefile instructions
# 	"make all"         :  Compiles all example programs, and places their binaries in the ./bin folder
#   "make [example]"   :  Compiles specific example program, and places its binary in the ./bin folder, example: "make simple_photo"

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++14 -I/Applications/Spinnaker/include -L/usr/local/lib -lSpinnaker -Wl,-rpath,/Applications/Spinnaker/lib

# Directories
SRC_DIR = ./src
EXAMPLES_DIR = ./examples
BIN_DIR = ./bin

# Source files for the library
LIB_SRC = $(SRC_DIR)/SpinnakerSDK_SpinCamera.cpp $(SRC_DIR)/SpinnakerSDK_SpinImage.cpp

# Example programs
EXAMPLES = $(wildcard $(EXAMPLES_DIR)/*.cpp)
EXAMPLE_PROGS = $(patsubst $(EXAMPLES_DIR)/%.cpp, $(BIN_DIR)/%, $(EXAMPLES))
EXAMPLE_TARGETS = $(patsubst $(EXAMPLES_DIR)/%.cpp, %, $(EXAMPLES))

# Default target
all: $(EXAMPLE_PROGS)

# Rule for building each example program
$(BIN_DIR)/%: $(EXAMPLES_DIR)/%.cpp $(LIB_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Add a target for each example program
$(EXAMPLE_TARGETS): %: $(BIN_DIR)/%

# Clean up
clean:
	rm -f $(BIN_DIR)/*

.PHONY: all clean $(EXAMPLE_TARGETS)