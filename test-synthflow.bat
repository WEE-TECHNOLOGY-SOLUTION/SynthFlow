@echo off
echo ========================================
echo   SynthFlow Installation Test
echo ========================================
echo.

echo Testing SynthFlow with default example...
echo.

dist\windows\synthflow.exe examples\hello.synth > output.txt 2>&1

if %ERRORLEVEL% EQU 0 (
    echo SUCCESS: SynthFlow is working correctly!
    echo.
    echo First few lines of output:
    powershell -Command "Get-Content output.txt -Head 10"
    echo ...
    echo.
    echo Full output saved to output.txt
) else (
    echo FAILED: SynthFlow encountered an error.
    type output.txt
)

echo.
echo Testing batch wrapper script...
echo.
dist\windows\synthflow.bat examples\hello.synth > wrapper-output.txt 2>&1

if %ERRORLEVEL% EQU 0 (
    echo SUCCESS: Wrapper script is working correctly!
) else (
    echo FAILED: Wrapper script encountered an error.
)

echo.
echo Cleaning up...
del output.txt wrapper-output.txt

echo.
echo Installation test completed!
pause