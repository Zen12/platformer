# Engine Core Module Tests

# Asset tests
add_executable(engine-asset-test
    asset_test.cpp
)

target_link_libraries(engine-asset-test PRIVATE
    engine-core
    doctest::doctest
)

add_test(NAME engine-asset-test COMMAND engine-asset-test)

# AI tests
add_executable(engine-ai-test
    astar_test.cpp
    bt_test.cpp
)

target_link_libraries(engine-ai-test PRIVATE
    engine-core
    doctest::doctest
)

add_test(NAME engine-ai-test COMMAND engine-ai-test)
