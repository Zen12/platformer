# Engine GUID Module Tests

add_executable(engine-guid-test
    guid_test.cpp
)

target_link_libraries(engine-guid-test PRIVATE
    engine-guid
    GTest::gtest_main
)

add_test(NAME engine-guid-test COMMAND engine-guid-test)
