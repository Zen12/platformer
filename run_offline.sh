#!/bin/bash

# Auto-detect platform
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Running on macOS"
    PLATFORM="DESKTOP_OSX"
    PARALLEL_FLAG="-j$(sysctl -n hw.ncpu)"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Running on Linux"
    PLATFORM="DESKTOP_LINUX"
    PARALLEL_FLAG="-j$(nproc)"
elif [[ "$OSTYPE" == "msys"* || "$OSTYPE" == "cygwin"* || "$OSTYPE" == "win32" ]]; then
    echo "Running on Windows"
    PLATFORM="DESKTOP_WINDOWS"
    PARALLEL_FLAG="/m"
else
    echo "Error: Unknown OS: $OSTYPE"
    exit 1
fi

# Set platform-specific flags
CMAKE_PLATFORM_FLAG="-DPLATFORM_${PLATFORM}=ON"

echo "Building in OFFLINE mode (will fail if dependencies not cached)"
echo "If this fails, run ./run_debug.sh first to download dependencies"

cmake -H. -Bbin/debug/desktop ${CMAKE_PLATFORM_FLAG} -DPLATFORM_DESKTOP=ON -DIS_PLATFORM_DEFINED=ON -DCMAKE_BUILD_TYPE=Debug -DOFFLINE_BUILD=ON &&
cmake --build bin/debug/desktop/ --target run -- $PARALLEL_FLAG
