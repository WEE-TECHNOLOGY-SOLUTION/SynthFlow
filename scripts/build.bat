@echo off
echo Building SynthFlow compiler...
cd /d D:\SynthFlow\synthflow
make clean
make all
if %ERRORLEVEL% EQU 0 (
    echo Build successful!
) else (
    echo Build failed!
    exit /b %ERRORLEVEL%
)