# SynthFlow Windows Installation Package

## Package Contents

The Windows installation package for SynthFlow includes:

1. **synthflow.exe** - The main SynthFlow lexer executable
2. **synthflow.bat** - Batch wrapper script for better user experience
3. **install.bat** - Windows installer script
4. **uninstall.bat** - Windows uninstaller script
5. **examples/** - Sample SynthFlow programs
6. **docs/** - Complete documentation
7. **README.md** - Project overview
8. **README.txt** - Package-specific instructions
9. **LICENSE** - License information
10. **VERSION** - Version information

## Installation Instructions

### Method 1: Automatic Installation
1. Extract the ZIP file
2. Right-click on `install.bat` and select "Run as administrator"
3. The installer will:
   - Copy files to `C:\Program Files\SynthFlow`
   - Add SynthFlow to your system PATH
   - Complete the installation

### Method 2: Manual Installation
1. Extract the ZIP file to your preferred location
2. Add the `bin` directory to your system PATH
3. Use SynthFlow from any command prompt

## Usage

After installation, you can use SynthFlow in several ways:

1. **Command-line usage**:
   ```
   synthflow myfile.synth
   ```

2. **Interactive mode** (double-click `synthflow.bat`):
   - Shows usage instructions
   - Automatically runs the example file
   - Keeps the window open for viewing results

3. **Drag and drop**:
   - Drag a `.synth` file onto `synthflow.bat`

## Package Details

- **Package Name**: synthflow-windows-1.0.0.zip
- **Size**: ~278 KB
- **Platform**: Windows 7/8/10/11
- **Architecture**: x64
- **Dependencies**: None

## Uninstallation

1. Right-click on `uninstall.bat` and select "Run as administrator"
2. Follow the prompts to complete uninstallation
3. Manually remove SynthFlow from your PATH if needed

## Verification

To verify the installation works correctly:
```
synthflow examples\hello.synth
```

This should tokenize the example file and display the lexical tokens.