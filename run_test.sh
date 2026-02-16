#!/bin/bash

# Build and run tests

set -e

BUILD_DIR="bin/test"
# Share dependency cache with debug build
FETCHCONTENT_BASE="$PWD/bin/debug/desktop/_deps"

echo "=== Building Tests ==="

# Configure with testing enabled
cmake -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON \
    -DIS_PLATFORM_DEFINED=ON \
    -DPLATFORM_DESKTOP=ON \
    -DPLATFORM_DESKTOP_OSX=ON \
    -DFETCHCONTENT_BASE_DIR="$FETCHCONTENT_BASE"

# Build
cmake --build "$BUILD_DIR" --parallel

echo ""
echo "=== Running Tests ==="

# Run engine module tests only (exclude RVO2 examples)
cd "$BUILD_DIR"
ctest --output-on-failure -R "engine-"
