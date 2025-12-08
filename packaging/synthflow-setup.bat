@echo off
cls
echo ========================================
echo   SynthFlow Programming Language Setup
echo ========================================
echo.
echo Welcome to the SynthFlow installer!
echo.
echo This script will install SynthFlow on your system.
echo.

REM Check for administrator privileges
net session >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: This installer requires administrator privileges.
    echo Please right-click and select "Run as administrator".
    echo.
    echo Press any key to exit...
    pause >nul
    exit /b 1
)

echo Checking system requirements...
echo.

REM Set installation directory
set INSTALL_DIR=C:\Program Files\SynthFlow
echo Installing to: %INSTALL_DIR%
echo.

REM Create installation directory
echo Creating installation directory...
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
if not exist "%INSTALL_DIR%\bin" mkdir "%INSTALL_DIR%\bin"
if not exist "%INSTALL_DIR%\examples" mkdir "%INSTALL_DIR%\examples"
if not exist "%INSTALL_DIR%\docs" mkdir "%INSTALL_DIR%\docs"

REM Copy files
echo Copying files...
xcopy /E /I /Y "..\dist\synthflow-windows\bin" "%INSTALL_DIR%\bin" >nul
xcopy /E /I /Y "..\dist\synthflow-windows\examples" "%INSTALL_DIR%\examples" >nul
xcopy /E /I /Y "..\dist\synthflow-windows\docs" "%INSTALL_DIR%\docs" >nul
copy /Y "..\dist\synthflow-windows\README.txt" "%INSTALL_DIR%\" >nul
copy /Y "..\dist\synthflow-windows\README.md" "%INSTALL_DIR%\" >nul
copy /Y "..\dist\synthflow-windows\LICENSE" "%INSTALL_DIR%\" >nul
copy /Y "..\dist\synthflow-windows\VERSION" "%INSTALL_DIR%\" >nul

REM Add to PATH
echo Adding SynthFlow to PATH...
setx PATH "%PATH%;%INSTALL_DIR%\bin" /M >nul

echo.
echo ========================================
echo   Installation Complete!
echo ========================================
echo.
echo SynthFlow has been successfully installed to:
echo %INSTALL_DIR%
echo.
echo The synthflow command is now available in your PATH.
echo.
echo To test the installation, open a new command prompt and run:
echo   synthflow examples\hello.synth
echo.
echo You can also double-click synthflow.bat in the installation directory
echo to run SynthFlow with the default example.
echo.
echo For more information, please read the documentation in:
echo %INSTALL_DIR%\docs
echo.
echo Press any key to exit...
pause >nul