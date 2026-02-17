# Engine AI Module Tests

add_executable(engine-ai-test
    astar_test.cpp
    bt_test.cpp
)

target_link_libraries(engine-ai-test PRIVATE
    engine-ai
    doctest::doctest
)

add_test(NAME engine-ai-test COMMAND engine-ai-test)
