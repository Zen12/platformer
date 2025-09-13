echo "Running WebAssembly build"
emcmake cmake -H. -Bbin/release/web -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release &&
emmake make -C bin/release/web -j$(nproc) &&
emrun --no_browser --port 8080 bin/release/web