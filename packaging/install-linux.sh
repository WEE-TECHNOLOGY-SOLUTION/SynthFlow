#!/bin/bash

# SynthFlow Linux Installer

echo "=================================="
echo "  SynthFlow Linux Installer"
echo "=================================="

# Check if running with sudo
if [[ $EUID -eq 0 ]]; then
   echo "This script should not be run as root" 1>&2
   exit 1
fi

# Set installation directory
INSTALL_DIR="/usr/local/synthflow"
echo "Installing to: $INSTALL_DIR"

# Create installation directory
sudo mkdir -p "$INSTALL_DIR"

# Copy files
echo "Copying files..."
sudo cp -r dist/linux/* "$INSTALL_DIR/"

# Make executable
sudo chmod +x "$INSTALL_DIR/synthflow"

# Create symlink in /usr/local/bin
echo "Creating symlink..."
sudo ln -sf "$INSTALL_DIR/synthflow" /usr/local/bin/synthflow

echo "=================================="
echo "  Installation Complete"
echo "=================================="
echo "SynthFlow has been installed to:"
echo "$INSTALL_DIR"
echo ""
echo "The synthflow command is now available in your PATH"
echo ""
echo "To test the installation, run:"
echo "synthflow examples/hello.synth"