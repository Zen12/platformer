# Engine Profiler Module Tests

add_executable(engine-profiler-test
    profiler_test.cpp
)

target_link_libraries(engine-profiler-test PRIVATE
    engine-profiler
    GTest::gtest_main
)

add_test(NAME engine-profiler-test COMMAND engine-profiler-test)
