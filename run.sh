cmake -H. -Bbin/release/desktop -DPLATFORM=Desktop -DCMAKE_BUILD_TYPE=Release &&
cmake --build ./bin/release/desktop &&
bin/release/desktop/Platformer
