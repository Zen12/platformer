#!/bin/bash
# Web platform auto-detected by CMake via CMAKE_SYSTEM_NAME=Emscripten
# No platform argument needed

PLATFORM="WEB"

# Auto-detect CPU count for parallel builds
if [[ "$OSTYPE" == "darwin"* ]]; then
    PARALLEL_FLAG="-j$(sysctl -n hw.ncpu)"
else
    PARALLEL_FLAG="-j$(nproc)"
fi

echo "Running WebAssembly build"
emcmake cmake -H. -Bbin/release/web -DPLATFORM_WEB=ON -DIS_PLATFORM_DEFINED=ON -DCMAKE_BUILD_TYPE=Release &&
emmake make -C bin/release/web $PARALLEL_FLAG &&
emrun --no_browser --port 8080 bin/release/web/output
