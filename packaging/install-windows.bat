@echo off
REM SynthFlow Windows Installer

echo ==================================
echo   SynthFlow Windows Installer
echo ==================================

REM Check if running as administrator
net session >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo This installer needs to be run as Administrator
    echo Right-click and select "Run as administrator"
    pause
    exit /b 1
)

REM Set installation directory
set INSTALL_DIR=C:\Program Files\SynthFlow
echo Installing to: %INSTALL_DIR%

REM Create installation directory
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

REM Copy files
echo Copying files...
xcopy /E /I /Y dist\windows\* "%INSTALL_DIR%" >nul

REM Add to PATH
echo Adding to PATH...
setx PATH "%PATH%;%INSTALL_DIR%" /M >nul

echo ==================================
echo   Installation Complete
echo ==================================
echo SynthFlow has been installed to:
echo %INSTALL_DIR%
echo.
echo The synthflow.exe command is now available in your PATH
echo.
echo To test the installation, open a new command prompt and run:
echo synthflow.exe examples/hello.synth
echo.
echo Press any key to exit...
pause >nul