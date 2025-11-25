@echo off
echo ==================================
echo   SynthFlow Installation Test
echo ==================================

echo Testing SynthFlow lexer...
dist\windows\synthflow.exe examples\hello.synth > test-output.txt

if %ERRORLEVEL% EQU 0 (
    echo SUCCESS: SynthFlow is working correctly!
    echo Sample output:
    type test-output.txt | findstr "Tokens"
    echo ...
    del test-output.txt
) else (
    echo FAILED: SynthFlow is not working correctly.
    if exist test-output.txt del test-output.txt
    exit /b 1
)

echo.
echo Installation test completed successfully!