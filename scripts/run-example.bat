@echo off
echo Running SynthFlow compiler on example file...
cd /d D:\SynthFlow\synthflow

if exist synthflow.exe (
    echo Compiling examples/hello.synth...
    synthflow.exe examples/hello.synth
    if %ERRORLEVEL% EQU 0 (
        echo Compilation successful!
    ) else (
        echo Compilation failed!
        exit /b %ERRORLEVEL%
    )
) else (
    echo Error: synthflow.exe not found. Please build the project first.
    exit /b 1
)