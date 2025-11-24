cmake_minimum_required(VERSION 3.10)
project(SynthFlow VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Include directories
include_directories(compiler/include)

# Lexer library
add_library(lexer
    compiler/src/lexer/lexer.cpp
)

# Main executable
add_executable(synthflow
    compiler/src/main.cpp
)

# Test executable
add_executable(test_lexer
    compiler/tests/test_lexer.cpp
)

# Link libraries
target_link_libraries(synthflow lexer)
target_link_libraries(test_lexer lexer)

# Install targets
install(TARGETS lexer DESTINATION lib)
install(DIRECTORY compiler/include/ DESTINATION include)