@echo off
echo ========================================
echo   SynthFlow Windows Installer
echo ========================================
echo.

REM Check if running as administrator
net session >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo This installer needs to be run as Administrator
    echo Right-click and select "Run as administrator"
    echo.
    echo Press any key to exit...
    pause >nul
    exit /b 1
)

REM Set installation directory
set INSTALL_DIR=C:\Program Files\SynthFlow
echo Installing to: %INSTALL_DIR%
echo.

REM Create installation directory
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

REM Copy files
echo Copying files...
xcopy /E /I /Y bin "%INSTALL_DIR%\bin" >nul
xcopy /E /I /Y examples "%INSTALL_DIR%\examples" >nul
xcopy /E /I /Y docs "%INSTALL_DIR%\docs" >nul
copy /Y README.md "%INSTALL_DIR%\" >nul
copy /Y LICENSE "%INSTALL_DIR%\" >nul

REM Add to PATH
echo Adding to PATH...
setx PATH "%PATH%;%INSTALL_DIR%\bin" /M >nul

echo.
echo ========================================
echo   Installation Complete
echo ========================================
echo SynthFlow has been installed to:
echo %INSTALL_DIR%
echo.
echo The synthflow command is now available in your PATH
echo.
echo To test the installation, open a new command prompt and run:
echo synthflow examples\hello.synth
echo.
echo Press any key to exit...
pause >nul