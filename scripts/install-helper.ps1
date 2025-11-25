# SynthFlow Installation Helper for Windows

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  SynthFlow Installation Helper" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "Step 1: Installing MinGW-w64" -ForegroundColor Yellow
Write-Host "----------------------------------------" -ForegroundColor Gray
Write-Host "Please follow these steps to install MinGW-w64:" -ForegroundColor White
Write-Host ""
Write-Host "1. Download MinGW-w64 from:" -ForegroundColor White
Write-Host "   https://github.com/brechtsanders/winlibs_mingw/releases" -ForegroundColor Green
Write-Host ""
Write-Host "2. Look for a file named something like:" -ForegroundColor White
Write-Host "   winlibs-x86_64-posix-seh-gcc-XX.X.X-llvm-XX.X.X-mingw-w64ucrt-XX.X.X-rX.7z" -ForegroundColor Green
Write-Host ""
Write-Host "3. Extract the downloaded file to C:\mingw64" -ForegroundColor White
Write-Host "   (You should have folders like C:\mingw64\bin, C:\mingw64\include, etc.)" -ForegroundColor White
Write-Host ""
Write-Host "4. Add C:\mingw64\bin to your system PATH:" -ForegroundColor White
Write-Host "   - Press Win + X and select 'System'" -ForegroundColor White
Write-Host "   - Click 'Advanced system settings'" -ForegroundColor White
Write-Host "   - Click 'Environment Variables'" -ForegroundColor White
Write-Host "   - Under 'System Variables', find and select 'Path', then click 'Edit'" -ForegroundColor White
Write-Host "   - Click 'New' and add 'C:\mingw64\bin'" -ForegroundColor White
Write-Host "   - Click OK to save" -ForegroundColor White
Write-Host ""
Write-Host "5. Restart your command prompt/PowerShell" -ForegroundColor White
Write-Host ""

Write-Host "Step 2: Verify Installation" -ForegroundColor Yellow
Write-Host "----------------------------------------" -ForegroundColor Gray
Write-Host "After installing MinGW-w64, open a NEW command prompt and run:" -ForegroundColor White
Write-Host ""
Write-Host "   g++ --version" -ForegroundColor Green
Write-Host "   make --version" -ForegroundColor Green
Write-Host ""

Write-Host "Step 3: Build SynthFlow" -ForegroundColor Yellow
Write-Host "----------------------------------------" -ForegroundColor Gray
Write-Host "Navigate to the SynthFlow directory and run:" -ForegroundColor White
Write-Host ""
Write-Host "   cd d:\SynthFlow\synthflow" -ForegroundColor Green
Write-Host "   make clean" -ForegroundColor Green
Write-Host "   make all" -ForegroundColor Green
Write-Host ""

Write-Host "Step 4: Run SynthFlow" -ForegroundColor Yellow
Write-Host "----------------------------------------" -ForegroundColor Gray
Write-Host "After successful compilation, run:" -ForegroundColor White
Write-Host ""
Write-Host "   synthflow.exe examples/hello.synth" -ForegroundColor Green
Write-Host ""

Write-Host "For detailed instructions, please refer to SYNTHFLOW_WINDOWS_INSTALLATION.md" -ForegroundColor Magenta
Write-Host ""

Write-Host "Press any key to continue..." -ForegroundColor Gray
$host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")