@echo off
REM Script to update the version in VERSION file and CMakeLists.txt

if "%~1"=="" (
    echo Usage: %0 ^<new_version^>
    exit /b 1
)

set NEW_VERSION=%1

REM Update VERSION file
echo %NEW_VERSION% > VERSION

REM Update CMakeLists.txt
powershell -Command "(gc CMakeLists.txt) -replace 'project\(SynthFlow VERSION .*', 'project(SynthFlow VERSION %NEW_VERSION%)' | Out-File -encoding UTF8 CMakeLists.txt"

echo Version updated to %NEW_VERSION%