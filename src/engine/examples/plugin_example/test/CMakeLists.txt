# Example Plugin Tests
# Demonstrates how to test plugin registrations

add_executable(engine-plugin-registration-test
    plugin_registration_test.cpp
)

target_include_directories(engine-plugin-registration-test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..
)

target_link_libraries(engine-plugin-registration-test PRIVATE
    engine-example-plugin
    engine-core
    EnTT::EnTT
    GTest::gtest_main
)

add_test(NAME engine-plugin-registration-test COMMAND engine-plugin-registration-test)
