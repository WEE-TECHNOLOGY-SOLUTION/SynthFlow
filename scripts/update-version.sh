#!/bin/bash
# Script to update the version in VERSION file and CMakeLists.txt

if [ $# -ne 1 ]; then
    echo "Usage: $0 <new_version>"
    exit 1
fi

NEW_VERSION=$1

# Update VERSION file
echo "$NEW_VERSION" > VERSION

# Update CMakeLists.txt
sed -i "s/project(SynthFlow VERSION .*)/project(SynthFlow VERSION $NEW_VERSION)/" CMakeLists.txt

echo "Version updated to $NEW_VERSION"