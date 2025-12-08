cmake_minimum_required(VERSION 3.10)
project(SynthFlow VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Include directories
include_directories(compiler/include)

# ------------------------------------------------------------------------------
# Modular Libraries
# ------------------------------------------------------------------------------

# Lexer
add_library(lexer compiler/src/lexer/lexer.cpp)

# AST
add_library(ast compiler/src/ast/ast_visitor.cpp)

# Parser
add_library(parser compiler/src/parser/parser.cpp)
target_link_libraries(parser ast lexer)

# Semantic
add_library(semantic compiler/src/semantic/semantic_analyzer.cpp)
target_link_libraries(semantic ast)

# Codegen
add_library(codegen compiler/src/codegen/code_generator.cpp)
target_link_libraries(codegen ast semantic)

# ------------------------------------------------------------------------------
# Executables
# ------------------------------------------------------------------------------

# Main Compiler Executable
add_executable(synthflow compiler/src/main.cpp)
target_link_libraries(synthflow parser semantic codegen ast lexer)

# LSP Server
add_executable(synthflow-lsp lsp-server/src/main.cpp)
target_link_libraries(synthflow-lsp parser semantic ast lexer)

# Test Executables
add_executable(test_lexer compiler/tests/test_lexer.cpp)
target_link_libraries(test_lexer lexer)

# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------
install(TARGETS synthflow synthflow-lsp lexer parser ast semantic codegen DESTINATION bin)
install(DIRECTORY compiler/include/ DESTINATION include)