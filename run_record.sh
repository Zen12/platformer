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
    echo "Warning: VIDEO_RECORDING not yet fully tested on Linux (FFmpeg configuration may need adjustments)"
elif [[ "$OSTYPE" == "msys"* || "$OSTYPE" == "cygwin"* || "$OSTYPE" == "win32" ]]; then
    echo "Running on Windows"
    PLATFORM="DESKTOP_WINDOWS"
    PARALLEL_FLAG="/m"
    echo "Warning: VIDEO_RECORDING not yet fully tested on Windows (FFmpeg configuration may need adjustments)"
else
    echo "Error: Unknown OS: $OSTYPE"
    exit 1
fi

# Set platform-specific flags + video recording
CMAKE_PLATFORM_FLAG="-DPLATFORM_${PLATFORM}=ON"

cmake -H. -Bbin/record/desktop ${CMAKE_PLATFORM_FLAG} -DPLATFORM_DESKTOP=ON -DCMAKE_BUILD_TYPE=Release -DVIDEO_RECORDING=ON &&
cmake --build bin/record/desktop --target run -- $PARALLEL_FLAG
