@echo off
REM Wrapper script for SynthFlow executable
REM This script handles double-click scenarios better

echo ========================================
echo   SynthFlow Programming Language
echo ========================================
echo.

if "%1"=="" (
    echo Usage: Drag and drop a .synth file onto this batch file
    echo Or run from command line: synthflow.bat ^<source_file^>
    echo.
    echo Running default example...
    echo.
    
    if exist "examples\hello.synth" (
        echo Analyzing examples\hello.synth:
        echo.
        dist\windows\synthflow.exe examples\hello.synth
    ) else (
        echo Error: Could not find examples\hello.synth
        echo Please provide a .synth file as an argument.
    )
) else (
    echo Analyzing %1:
    echo.
    dist\windows\synthflow.exe "%1"
)

echo.
echo Press any key to exit...
pause >nul