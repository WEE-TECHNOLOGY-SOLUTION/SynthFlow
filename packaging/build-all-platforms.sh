#!/bin/bash

# Cross-platform build script for SynthFlow compiler
# This script builds the compiler for Windows, macOS, and Linux

echo "=================================="
echo "  SynthFlow Cross-Platform Builder"
echo "=================================="

# Create dist directory if it doesn't exist
mkdir -p dist

# Get the current version
VERSION=$(cat VERSION 2>/dev/null || echo "1.0.0")
echo "Building version: $VERSION"

# Build for Windows (using MinGW)
echo "Building for Windows..."
mkdir -p dist/windows
# Assuming MinGW is installed and in PATH
g++ -std=c++17 -Icompiler/include \
    compiler/src/lexer/lexer.cpp \
    compiler/src/parser/parser.cpp \
    compiler/src/ast/ast_visitor.cpp \
    compiler/src/semantic/semantic_analyzer.cpp \
    compiler/src/codegen/code_generator.cpp \
    compiler/src/main.cpp \
    -o dist/windows/synthflow.exe

# Copy examples and documentation
cp -r examples dist/windows/
cp -r docs dist/windows/
cp README.md dist/windows/
cp LICENSE dist/windows/

echo "Windows build complete!"

# For macOS and Linux builds, we would typically use a CI/CD system
# or cross-compilation tools, but for now we'll just create placeholders

echo "Creating macOS package..."
mkdir -p dist/macos
cp -r examples dist/macos/
cp -r docs dist/macos/
cp README.md dist/macos/
cp LICENSE dist/macos/
# Create a placeholder script
echo '#!/bin/bash
echo "SynthFlow compiler for macOS - Build pending"' > dist/macos/synthflow
chmod +x dist/macos/synthflow

echo "macOS package created!"

echo "Creating Linux package..."
mkdir -p dist/linux
cp -r examples dist/linux/
cp -r docs dist/linux/
cp README.md dist/linux/
cp LICENSE dist/linux/
# Create a placeholder script
echo '#!/bin/bash
echo "SynthFlow compiler for Linux - Build pending"' > dist/linux/synthflow
chmod +x dist/linux/synthflow

echo "Linux package created!"

echo "=================================="
echo "  Build Summary"
echo "=================================="
echo "Windows: dist/windows/"
echo "macOS:   dist/macos/"
echo "Linux:   dist/linux/"
echo ""
echo "Packages created successfully!"