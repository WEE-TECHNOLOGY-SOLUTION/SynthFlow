# Simple Makefile for SynthFlow compiler

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -Icompiler/include
LIBS = 

# Directories
SRC_DIR = compiler/src
INC_DIR = compiler/include
LEXER_DIR = $(SRC_DIR)/lexer
PARSER_DIR = $(SRC_DIR)/parser
AST_DIR = $(SRC_DIR)/ast
SEMANTIC_DIR = $(SRC_DIR)/semantic
CODEGEN_DIR = $(SRC_DIR)/codegen
TEST_DIR = tests

# Source files
LEXER_SRC = $(LEXER_DIR)/lexer.cpp
PARSER_SRC = $(PARSER_DIR)/parser.cpp
AST_SRC = $(AST_DIR)/ast_visitor.cpp
SEMANTIC_SRC = $(SEMANTIC_DIR)/semantic_analyzer.cpp
CODEGEN_SRC = $(CODEGEN_DIR)/code_generator.cpp
MAIN_SRC = $(SRC_DIR)/main.cpp
TEST_SRC = $(TEST_DIR)/test_lexer.cpp
TEST_PARSER_SRC = $(TEST_DIR)/test_parser.cpp
TEST_SEMANTIC_SRC = $(TEST_DIR)/test_semantic.cpp
TEST_CODEGEN_SRC = $(TEST_DIR)/test_codegen.cpp
TEST_WHILE_SRC = $(TEST_DIR)/test_while_loop.cpp
TEST_BREAK_CONTINUE_SRC = $(TEST_DIR)/test_break_continue.cpp
TEST_FOR_SRC = $(TEST_DIR)/test_for_loop.cpp
TEST_ARRAYS_SRC = $(TEST_DIR)/test_arrays.cpp

# Object files
LEXER_OBJ = lexer.o
PARSER_OBJ = parser.o
AST_OBJ = ast_visitor.o
SEMANTIC_OBJ = semantic_analyzer.o
CODEGEN_OBJ = code_generator.o
MAIN_OBJ = main.o
TEST_OBJ = test_lexer.o
TEST_PARSER_OBJ = test_parser.o
TEST_SEMANTIC_OBJ = test_semantic.o
TEST_CODEGEN_OBJ = test_codegen.o
TEST_WHILE_OBJ = test_while_loop.o
TEST_BREAK_CONTINUE_OBJ = test_break_continue.o
TEST_FOR_OBJ = test_for_loop.o
TEST_ARRAYS_OBJ = test_arrays.o

# Executables
MAIN_EXE = synthflow.exe
TEST_EXE = test_lexer.exe
TEST_PARSER_EXE = test_parser.exe
TEST_SEMANTIC_EXE = test_semantic.exe
TEST_CODEGEN_EXE = test_codegen.exe
TEST_WHILE_EXE = test_while_loop.exe
TEST_BREAK_CONTINUE_EXE = test_break_continue.exe
TEST_FOR_EXE = test_for_loop.exe
TEST_ARRAYS_EXE = test_arrays.exe

# Default target
all: $(MAIN_EXE) $(TEST_EXE) $(TEST_PARSER_EXE) $(TEST_SEMANTIC_EXE) $(TEST_CODEGEN_EXE) $(TEST_WHILE_EXE) $(TEST_BREAK_CONTINUE_EXE) $(TEST_FOR_EXE) $(TEST_ARRAYS_EXE)

# Main executable
$(MAIN_EXE): $(MAIN_OBJ) $(LEXER_OBJ) $(PARSER_OBJ) $(AST_OBJ) $(SEMANTIC_OBJ) $(CODEGEN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Test executables
$(TEST_EXE): $(TEST_OBJ) $(LEXER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(TEST_PARSER_EXE): $(TEST_PARSER_OBJ) $(LEXER_OBJ) $(PARSER_OBJ) $(AST_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(TEST_SEMANTIC_EXE): $(TEST_SEMANTIC_OBJ) $(LEXER_OBJ) $(PARSER_OBJ) $(AST_OBJ) $(SEMANTIC_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(TEST_CODEGEN_EXE): $(TEST_CODEGEN_OBJ) $(LEXER_OBJ) $(PARSER_OBJ) $(AST_OBJ) $(CODEGEN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(TEST_WHILE_EXE): $(TEST_WHILE_OBJ) $(LEXER_OBJ) $(PARSER_OBJ) $(AST_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(TEST_BREAK_CONTINUE_EXE): $(TEST_BREAK_CONTINUE_OBJ) $(LEXER_OBJ) $(PARSER_OBJ) $(AST_OBJ) $(SEMANTIC_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(TEST_FOR_EXE): $(TEST_FOR_OBJ) $(LEXER_OBJ) $(PARSER_OBJ) $(AST_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(TEST_ARRAYS_EXE): $(TEST_ARRAYS_OBJ) $(LEXER_OBJ) $(PARSER_OBJ) $(AST_OBJ) $(SEMANTIC_OBJ) $(CODEGEN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Object files
$(LEXER_OBJ): $(LEXER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(PARSER_OBJ): $(PARSER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(AST_OBJ): $(AST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(SEMANTIC_OBJ): $(SEMANTIC_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(CODEGEN_OBJ): $(CODEGEN_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(MAIN_OBJ): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_OBJ): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_PARSER_OBJ): $(TEST_PARSER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_SEMANTIC_OBJ): $(TEST_SEMANTIC_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_CODEGEN_OBJ): $(TEST_CODEGEN_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_WHILE_OBJ): $(TEST_WHILE_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_BREAK_CONTINUE_OBJ): $(TEST_BREAK_CONTINUE_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_FOR_OBJ): $(TEST_FOR_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_ARRAYS_OBJ): $(TEST_ARRAYS_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	del *.o $(MAIN_EXE) $(TEST_EXE) $(TEST_PARSER_EXE) $(TEST_SEMANTIC_EXE) $(TEST_CODEGEN_EXE) $(TEST_WHILE_EXE) $(TEST_BREAK_CONTINUE_EXE) $(TEST_FOR_EXE) $(TEST_ARRAYS_EXE) test-installation.bat test-synthflow.bat simple_test.exe 2>nul || true

# Run tests
test: $(TEST_EXE) $(TEST_PARSER_EXE) $(TEST_SEMANTIC_EXE) $(TEST_CODEGEN_EXE) $(TEST_WHILE_EXE) $(TEST_BREAK_CONTINUE_EXE) $(TEST_FOR_EXE) $(TEST_ARRAYS_EXE)
	./$(TEST_EXE)
	./$(TEST_PARSER_EXE)
	./$(TEST_SEMANTIC_EXE)
	./$(TEST_CODEGEN_EXE)
	./$(TEST_WHILE_EXE)
	./$(TEST_BREAK_CONTINUE_EXE)
	./$(TEST_FOR_EXE)
	./$(TEST_ARRAYS_EXE)

.PHONY: all clean test