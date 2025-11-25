# PowerShell script to download and install MinGW-w64
Write-Host "Downloading and installing MinGW-w64..." -ForegroundColor Yellow

# Create a temporary directory for downloading
$tempDir = "$env:TEMP\SynthFlow_MinGW"
New-Item -ItemType Directory -Path $tempDir -Force | Out-Null

# Download MinGW-w64 (using a known working version)
$mingwUrl = "https://github.com/brechtsanders/winlibs_mingw/releases/download/13.2.0posix-17.0.6-11.0.1-ucrt-r1/winlibs-x86_64-posix-seh-gcc-13.2.0-llvm-17.0.6-mingw-w64ucrt-11.0.1-r1.7z"
$downloadPath = "$tempDir\mingw.7z"

Write-Host "Downloading MinGW-w64 from $mingwUrl..." -ForegroundColor Cyan
Invoke-WebRequest -Uri $mingwUrl -OutFile $downloadPath

Write-Host "Extracting MinGW-w64..." -ForegroundColor Cyan
# Extract using 7-Zip (assuming it's installed)
$7zPath = "C:\Program Files\7-Zip\7z.exe"
if (Test-Path $7zPath) {
    & $7zPath x $downloadPath -o"$tempDir\mingw64"
} else {
    # Try using built-in tar command (Windows 10+)
    mkdir "$tempDir\mingw64" | Out-Null
    tar -xf $downloadPath -C "$tempDir\mingw64"
}

# Move to Program Files
$installPath = "C:\mingw64"
Write-Host "Installing MinGW-w64 to $installPath..." -ForegroundColor Cyan
Move-Item -Path "$tempDir\mingw64\*" -Destination $installPath -Force

# Add to PATH
$currentPath = [System.Environment]::GetEnvironmentVariable("PATH", "Machine")
if (-not ($currentPath -like "*$installPath\bin*")) {
    $newPath = "$installPath\bin;$currentPath"
    [System.Environment]::SetEnvironmentVariable("PATH", $newPath, "Machine")
    Write-Host "Added $installPath\bin to system PATH" -ForegroundColor Green
}

# Clean up
Remove-Item -Path $tempDir -Recurse -Force

Write-Host "MinGW-w64 installation completed!" -ForegroundColor Green
Write-Host "Please restart your terminal/command prompt to use the new PATH." -ForegroundColor Yellow