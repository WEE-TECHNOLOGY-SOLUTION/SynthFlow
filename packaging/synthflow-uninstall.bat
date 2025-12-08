@echo off
cls
echo ========================================
echo   SynthFlow Programming Language Uninstaller
echo ========================================
echo.
echo This script will uninstall SynthFlow from your system.
echo.

REM Check for administrator privileges
net session >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: This uninstaller requires administrator privileges.
    echo Please right-click and select "Run as administrator".
    echo.
    echo Press any key to exit...
    pause >nul
    exit /b 1
)

echo Are you sure you want to uninstall SynthFlow? (Y/N)
set /p CONFIRM=
if /i "%CONFIRM%" NEQ "Y" (
    echo Uninstallation cancelled.
    echo.
    echo Press any key to exit...
    pause >nul
    exit /b 0
)

REM Set installation directory
set INSTALL_DIR=C:\Program Files\SynthFlow
echo.
echo Uninstalling from: %INSTALL_DIR%
echo.

REM Remove installation directory
echo Removing files...
if exist "%INSTALL_DIR%" (
    rmdir /S /Q "%INSTALL_DIR%"
    echo Files removed.
) else (
    echo Installation directory not found.
)

echo.
echo ========================================
echo   Manual Cleanup Required
echo ========================================
echo.
echo Please manually remove "%INSTALL_DIR%\bin" from your PATH:
echo.
echo 1. Press Win + X and select "System"
echo 2. Click "Advanced system settings"
echo 3. Click "Environment Variables"
echo 4. Under "System Variables", find and select "Path", then click "Edit"
echo 5. Find and remove the entry: %INSTALL_DIR%\bin
echo 6. Click OK to save
echo.
echo Press any key to exit...
pause >nul