@echo off
REM Windows build script for SynthFlow compiler

echo ==================================
echo   SynthFlow Windows Builder
echo ==================================

REM Create dist directory if it doesn't exist
if not exist "dist" mkdir dist

REM Get the current version
set /p VERSION=<VERSION
if "%VERSION%"=="" set VERSION=1.0.0
echo Building version: %VERSION%

REM Build for Windows (using MinGW or Visual Studio)
echo Building for Windows...
if not exist "dist\windows" mkdir dist\windows

REM Check if g++ is available (MinGW)
g++ --version >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo Using MinGW compiler...
    REM Build a minimal version that works
    g++ -std=c++17 -Icompiler/include ^
        minimal_main.cpp ^
        compiler/src/lexer/lexer.cpp ^
        -o dist/windows/synthflow.exe
) else (
    REM Check if Visual Studio cl is available
    cl /? >nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo Using Visual Studio compiler...
        cl /std:c++17 /Icompiler/include /EHsc ^
            minimal_main.cpp ^
            compiler/src/lexer/lexer.cpp ^
            /link /out:dist/windows/synthflow.exe
    ) else (
        echo Error: No C++ compiler found!
        echo Please install MinGW-w64 or Visual Studio
        exit /b 1
    )
)

REM Check if build was successful
if exist "dist/windows/synthflow.exe" (
    echo Windows build successful!
) else (
    echo Windows build failed!
    exit /b 1
)

REM Copy examples and documentation
echo Copying examples and documentation...
xcopy /E /I /Y examples dist\windows\examples >nul
xcopy /E /I /Y docs dist\windows\docs >nul
copy /Y README.md dist\windows\ >nul
copy /Y LICENSE dist\windows\ >nul
copy /Y synthflow.bat dist\windows\ >nul

echo ==================================
echo   Windows Build Complete
echo ==================================
echo Executable: dist/windows/synthflow.exe
echo Examples:   dist/windows/examples/
echo Docs:       dist/windows/docs/
echo Batch file: dist/windows/synthflow.bat
echo ==================================