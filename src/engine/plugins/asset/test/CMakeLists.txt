# Engine Asset Module Tests

add_executable(engine-asset-test
    asset_test.cpp
)

target_link_libraries(engine-asset-test PRIVATE
    engine-asset
    doctest::doctest
)

add_test(NAME engine-asset-test COMMAND engine-asset-test)
