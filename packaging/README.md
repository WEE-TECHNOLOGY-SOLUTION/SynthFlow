# SynthFlow Packaging

This directory contains scripts and configuration files for creating installable packages of the SynthFlow programming language for various platforms.

## Package Types

### Windows
- `build-windows.bat` - Builds the Windows executable
- `install-windows.bat` - Installs SynthFlow on Windows
- `synthflow-windows.nsi` - NSIS installer script for creating a Windows installer

### macOS
- `install-macos.sh` - Installs SynthFlow on macOS

### Linux
- `install-linux.sh` - Installs SynthFlow on Linux
- `synthflow-debian.sh` - Creates a Debian package (.deb)

## Building Packages

### Windows

1. Build the Windows executable:
   ```
   packaging\build-windows.bat
   ```

2. Create the installer (requires NSIS):
   ```
   makensis packaging\synthflow-windows.nsi
   ```

### macOS

1. Build the macOS executable (requires cross-compilation tools)
2. Run the installer script:
   ```
   sudo packaging/install-macos.sh
   ```

### Linux

1. Build the Linux executable (requires cross-compilation tools)
2. Run the installer script:
   ```
   sudo packaging/install-linux.sh
   ```

3. Create Debian package:
   ```
   packaging/synthflow-debian.sh
   ```

## Cross-Platform Build

The `build-all-platforms.sh` script attempts to build for all platforms, though cross-compilation may require additional tools.

## Directory Structure

```
dist/
├── windows/     # Windows build output
├── macos/       # macOS build output
├── linux/       # Linux build output
└── *.exe        # Windows installer
```