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

cmake -H. -Bbin/release/desktop ${CMAKE_PLATFORM_FLAG} -DPLATFORM_DESKTOP=ON -DIS_PLATFORM_DEFINED=ON -DCMAKE_BUILD_TYPE=Release &&
cmake --build bin/release/desktop --target run -- $PARALLEL_FLAG
