@echo off
echo ========================================
echo   SynthFlow Package Test
echo ========================================
echo.

echo Testing package structure...
if exist synthflow-windows\bin\synthflow.exe (
    echo [PASS] synthflow.exe found
) else (
    echo [FAIL] synthflow.exe not found
    exit /b 1
)

if exist synthflow-windows\bin\synthflow.bat (
    echo [PASS] synthflow.bat found
) else (
    echo [FAIL] synthflow.bat not found
    exit /b 1
)

if exist synthflow-windows\examples\hello.synth (
    echo [PASS] example files found
) else (
    echo [FAIL] example files not found
    exit /b 1
)

if exist synthflow-windows\docs\SYNTHFLOW_USAGE_GUIDE.md (
    echo [PASS] documentation files found
) else (
    echo [FAIL] documentation files not found
    exit /b 1
)

if exist synthflow-windows\README.txt (
    echo [PASS] README.txt found
) else (
    echo [FAIL] README.txt not found
    exit /b 1
)

if exist synthflow-windows\install.bat (
    echo [PASS] install.bat found
) else (
    echo [FAIL] install.bat not found
    exit /b 1
)

if exist synthflow-windows\uninstall.bat (
    echo [PASS] uninstall.bat found
) else (
    echo [FAIL] uninstall.bat not found
    exit /b 1
)

echo.
echo [SUCCESS] All package files are present!
echo.
echo Package size: 
for /f %i in ('dir /b synthflow-windows-1.0.0.zip') do echo %~zi bytes
echo.
echo To test the installation:
echo 1. Extract the ZIP file
echo 2. Run install.bat as Administrator
echo 3. Open a new command prompt
echo 4. Run: synthflow examples\hello.synth