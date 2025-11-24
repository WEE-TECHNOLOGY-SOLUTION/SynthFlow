@echo off
REM Build script for SynthFlow compiler on Windows

REM Check if make is available
where make >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: Make is not installed or not in PATH
    echo Please install MinGW or another build tool with make support
    exit /b 1
)

REM Build the project
make all

REM Run tests
if exist test_lexer.exe (
    test_lexer.exe
) else (
    echo Error: test_lexer.exe not found
    exit /b 1
)