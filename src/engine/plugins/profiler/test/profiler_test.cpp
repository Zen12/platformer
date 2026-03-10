#include <gtest/gtest.h>
#include "profiler.hpp"
#include <string>
#include <regex>

TEST(Profiler, GeneratesTimestampedFilename) {
    std::string filename = Profiler::GenerateTimestampedFilename();

    EXPECT_FALSE(filename.empty());
    EXPECT_EQ(filename.find("profile_"), 0);
    EXPECT_NE(filename.find(".json"), std::string::npos);
}

TEST(Profiler, FilenameFormatIsCorrect) {
    std::string filename = Profiler::GenerateTimestampedFilename();

    // Format: profile_YYYYMMDD_HHMMSS_mmm.json
    std::regex pattern(R"(profile_\d{8}_\d{6}_\d{3}\.json)");
    EXPECT_TRUE(std::regex_match(filename, pattern));
}

TEST(Profiler, ProfileScopeMacroCompiles) {
    {
        PROFILE_SCOPE("test_scope");
        int x = 1 + 1;
        EXPECT_EQ(x, 2);
    }
}

TEST(Profiler, ProfileScopeCatMacroCompiles) {
    {
        PROFILE_SCOPE_CAT("TestCategory", "test_scope");
        int x = 2 + 2;
        EXPECT_EQ(x, 4);
    }
}
