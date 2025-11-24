#!/bin/bash
# Build script for SynthFlow compiler

# Check if make is available
if ! command -v make &> /dev/null; then
    echo "Error: Make is not installed"
    echo "Please install build tools (e.g., sudo apt install build-essential)"
    exit 1
fi

# Build the project
make all

# Run tests
if [ -f "test_lexer" ]; then
    ./test_lexer
else
    echo "Error: test_lexer not found"
    exit 1
fi