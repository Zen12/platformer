#pragma once

// Profiler wrapper for minitrace
// Outputs Chrome Tracing JSON format for analysis
// View traces in chrome://tracing or https://ui.perfetto.dev

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

// MTR_ENABLED is defined in CMakeLists.txt for debug builds
#include <minitrace.h>

// Map PROFILE_SCOPE to MTR_SCOPE for compatibility with existing code
#define PROFILE_SCOPE(name) MTR_SCOPE("Engine", name)
#define PROFILE_SCOPE_CAT(category, name) MTR_SCOPE(category, name)

namespace Profiler {

inline std::string GenerateTimestampedFilename() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    std::ostringstream oss;
    oss << "profile_"
        << std::put_time(&tm, "%Y%m%d_%H%M%S")
        << "_" << std::setfill('0') << std::setw(3) << ms.count()
        << ".json";

    return oss.str();
}

inline void Init([[maybe_unused]] const std::string& outputDir = ".") {
#ifdef MTR_ENABLED
    std::string filename = GenerateTimestampedFilename();
    std::string fullPath = outputDir + "/" + filename;

    mtr_init(fullPath.c_str());
    mtr_register_sigint_handler();
    MTR_META_PROCESS_NAME("GameEngine");
    MTR_META_THREAD_NAME("Main");
#endif
}

inline void Shutdown() {
#ifdef MTR_ENABLED
    mtr_flush();
    mtr_shutdown();
#endif
}

inline void Flush() {
#ifdef MTR_ENABLED
    mtr_flush();
#endif
}

} // namespace Profiler
