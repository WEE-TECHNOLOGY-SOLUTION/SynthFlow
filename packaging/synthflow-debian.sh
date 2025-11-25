#!/bin/bash

# Create Debian package for SynthFlow

echo "Creating Debian package for SynthFlow..."

# Get version
VERSION=$(cat ../VERSION 2>/dev/null || echo "1.0.0")
echo "Package version: $VERSION"

# Create package directory structure
mkdir -p debian/synthflow-$VERSION
mkdir -p debian/synthflow-$VERSION/DEBIAN
mkdir -p debian/synthflow-$VERSION/usr/bin
mkdir -p debian/synthflow-$VERSION/usr/share/synthflow
mkdir -p debian/synthflow-$VERSION/usr/share/doc/synthflow

# Create control file
cat > debian/synthflow-$VERSION/DEBIAN/control << EOF
Package: synthflow
Version: $VERSION
Section: devel
Priority: optional
Architecture: amd64
Depends: libc6 (>= 2.2.5)
Maintainer: SynthFlow Team <synthflow@example.com>
Description: An AI-native programming language
 SynthFlow is a modern programming language designed for AI development
 with clean syntax and powerful constructs for modern software development.
EOF

# Copy files
cp -r ../dist/linux/* debian/synthflow-$VERSION/usr/share/synthflow/
cp ../LICENSE debian/synthflow-$VERSION/usr/share/doc/synthflow/copyright

# Create symlink script
cat > debian/synthflow-$VERSION/DEBIAN/postinst << EOF
#!/bin/bash
ln -sf /usr/share/synthflow/synthflow /usr/bin/synthflow
EOF

chmod 755 debian/synthflow-$VERSION/DEBIAN/postinst

# Create removal script
cat > debian/synthflow-$VERSION/DEBIAN/postrm << EOF
#!/bin/bash
rm -f /usr/bin/synthflow
EOF

chmod 755 debian/synthflow-$VERSION/DEBIAN/postrm

# Set permissions
find debian/synthflow-$VERSION -type d | xargs chmod 755
find debian/synthflow-$VERSION -type f | xargs chmod 644
chmod 755 debian/synthflow-$VERSION/usr/share/synthflow/synthflow

# Build package
dpkg-deb --build debian/synthflow-$VERSION ../dist/synthflow-$VERSION-amd64.deb

echo "Debian package created: dist/synthflow-$VERSION-amd64.deb"