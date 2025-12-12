cmake_minimum_required(VERSION 3.10)
project(SynthFlow VERSION 0.0.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
endif()

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

# Codegen (Renamed to synthflow_codegen to avoid policy CMP0171)
add_library(synthflow_codegen compiler/src/codegen/code_generator.cpp)
target_link_libraries(synthflow_codegen ast semantic)

# HTTP Client
add_library(http_client compiler/src/http/http_client.cpp)
if(WIN32)
    target_link_libraries(http_client winhttp)
elseif(APPLE)
    # On macOS, use system frameworks for HTTP
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    find_library(SECURITY_FRAMEWORK Security)
    # For now, just compile the stub version
endif()

# Interpreter
add_library(interpreter compiler/src/interpreter/interpreter.cpp)
target_link_libraries(interpreter ast http_client)

# JavaScript Transpiler
add_library(js_transpiler compiler/src/codegen/js_transpiler.cpp)
target_link_libraries(js_transpiler ast)

# ------------------------------------------------------------------------------
# Executables
# ------------------------------------------------------------------------------

# Main Compiler Executable
add_executable(synthflow compiler/src/main.cpp)
target_link_libraries(synthflow parser semantic synthflow_codegen interpreter js_transpiler ast lexer http_client)

# LSP Server
add_executable(synthflow-lsp lsp-server/src/main.cpp)
target_link_libraries(synthflow-lsp parser semantic ast lexer)

# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------
install(TARGETS synthflow synthflow-lsp lexer parser ast semantic synthflow_codegen interpreter http_client DESTINATION bin)
install(DIRECTORY compiler/include/ DESTINATION include)