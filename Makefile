# Simple Makefile for SynthFlow compiler

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -Icompiler/include
LIBS = 

# Directories
SRC_DIR = compiler/src
INC_DIR = compiler/include
LEXER_DIR = $(SRC_DIR)/lexer
TEST_DIR = compiler/tests

# Source files
LEXER_SRC = $(LEXER_DIR)/lexer.cpp
MAIN_SRC = $(SRC_DIR)/main.cpp
TEST_SRC = $(TEST_DIR)/test_lexer.cpp

# Object files
LEXER_OBJ = lexer.o
MAIN_OBJ = main.o
TEST_OBJ = test_lexer.o

# Executables
MAIN_EXE = synthflow.exe
TEST_EXE = test_lexer.exe

# Default target
all: $(MAIN_EXE) $(TEST_EXE)

# Main executable
$(MAIN_EXE): $(MAIN_OBJ) $(LEXER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Test executable
$(TEST_EXE): $(TEST_OBJ) $(LEXER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Object files
$(LEXER_OBJ): $(LEXER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(MAIN_OBJ): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_OBJ): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	del *.o $(MAIN_EXE) $(TEST_EXE) 2>nul || true

# Run tests
test: $(TEST_EXE)
	./$(TEST_EXE)

.PHONY: all clean test