if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Running on macOS" &&
    cmake -H. -Bbin/release/desktop -DPLATFORM=Desktop -DCMAKE_BUILD_TYPE=Release &&
    cmake --build ./bin/release/desktop &&
    bin/release/desktop/Platformer
elif [[ "$OSTYPE" == "msys"* || "$OSTYPE" == "cygwin"* || "$OSTYPE" == "win32" ]]; then
    echo "Running on Windows"
else
    echo "Running on an unknown OS: $OSTYPE"
fi
