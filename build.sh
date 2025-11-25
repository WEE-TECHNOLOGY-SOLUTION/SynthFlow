#!/bin/bash

# Build script for SynthFlow compiler

echo "Building SynthFlow compiler..."

# Clean previous build
make clean

# Build all targets
make all

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Run 'make test' to execute tests"
    echo "Run './synthflow.exe examples/hello.synth' to compile an example"
else
    echo "Build failed!"
    exit 1
fi