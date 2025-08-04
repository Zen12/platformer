if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Running on macOS" &&
    cmake -H. -Bbin/debug/desktop -DPLATFORM=Desktop -DCMAKE_BUILD_TYPE=Debug -DCMAKE_SKIP_TEST_ALL_DEPENDENCY=OFF &&
    cmake --build bin/debug/desktop/ &&
    bin/debug/desktop/Platformer
elif [[ "$OSTYPE" == "msys"* || "$OSTYPE" == "cygwin"* || "$OSTYPE" == "win32" ]]; then
    echo "Running on Windows"
else
    echo "Running on an unknown OS: $OSTYPE"
fi
