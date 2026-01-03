#!/bin/bash
# Web platform auto-detected by CMake via CMAKE_SYSTEM_NAME=Emscripten
# No platform argument needed

PLATFORM="WEB"

echo "Running WebAssembly build"
emcmake cmake -H. -Bbin/release/web -DPLATFORM_WEB=ON -DIS_PLATFORM_DEFINED=ON -DCMAKE_BUILD_TYPE=Release &&
emmake make -C bin/release/web -j$(nproc) &&
emrun --no_browser --port 8080 bin/release/web/output
