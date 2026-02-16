#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "profiler.hpp"
#include <string>
#include <regex>

TEST_CASE("Profiler generates timestamped filename") {
    std::string filename = Profiler::GenerateTimestampedFilename();

    CHECK_FALSE(filename.empty());
    CHECK(filename.find("profile_") == 0);
    CHECK(filename.find(".json") != std::string::npos);
}

TEST_CASE("Profiler filename format is correct") {
    std::string filename = Profiler::GenerateTimestampedFilename();

    // Format: profile_YYYYMMDD_HHMMSS_mmm.json
    std::regex pattern(R"(profile_\d{8}_\d{6}_\d{3}\.json)");
    CHECK(std::regex_match(filename, pattern));
}

TEST_CASE("PROFILE_SCOPE macro compiles") {
    // Just verify the macro compiles without errors
    // In non-debug builds, it should be a no-op
    {
        PROFILE_SCOPE("test_scope");
        int x = 1 + 1;
        CHECK(x == 2);
    }
}

TEST_CASE("PROFILE_SCOPE_CAT macro compiles") {
    {
        PROFILE_SCOPE_CAT("TestCategory", "test_scope");
        int x = 2 + 2;
        CHECK(x == 4);
    }
}
