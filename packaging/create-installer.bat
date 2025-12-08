@echo off
echo ========================================
echo   SynthFlow Installer Creator
echo ========================================
echo.

REM Check if 7-Zip is available
7z >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: 7-Zip is not installed or not in PATH
    echo Please install 7-Zip from https://www.7-zip.org/
    echo.
    echo Press any key to exit...
    pause >nul
    exit /b 1
)

REM Create installer directory if it doesn't exist
if not exist "..\releases" mkdir "..\releases"

REM Create a self-extracting installer using 7-Zip
echo Creating self-extracting installer...
7z a -sfx "..\releases\synthflow-1.0.0-installer.exe" "..\dist\synthflow-windows\*"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo   Installer created successfully!
    echo ========================================
    echo Installer location: ..\releases\synthflow-1.0.0-installer.exe
    echo.
    echo This is a self-extracting installer that will:
    echo 1. Extract SynthFlow to C:\Program Files\SynthFlow
    echo 2. Add SynthFlow to your PATH
    echo 3. Create shortcuts in the Start Menu
    echo.
    echo To install, simply run the executable as Administrator.
    echo.
) else (
    echo.
    echo Error: Failed to create installer
    echo.
)

echo Press any key to exit...
pause >nul