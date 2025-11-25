@echo off
echo Building SynthFlow compiler...
cd /d D:\SynthFlow\synthflow
make clean
make all
if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Run "make test" to execute tests
    echo Run "synthflow.exe examples/hello.synth" to compile an example
) else (
    echo Build failed!
    exit /b %ERRORLEVEL%
)