cmake -H. -Bbin/debug/desktop -DPLATFORM=Desktop -DCMAKE_BUILD_TYPE=Debug &&
cmake --build bin/debug/desktop/ &&
bin/debug/desktop/Platformer
