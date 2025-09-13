#pragma once

#include <filesystem>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#include <crt_externs.h>
#else
#include <unistd.h>
#include <limits.h>
#include <fstream>
#endif

inline std::filesystem::path GetCommandLinePath() {
#ifdef __EMSCRIPTEN__
    // Web: ignore command line, return empty
    return {};
#elif _WIN32
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    std::filesystem::path result;
    if (argc > 1) result = argv[1]; // first argument
    LocalFree(argv);
    return result;
#elif __APPLE__
    int argc = *_NSGetArgc();
    char** argv = *_NSGetArgv();
    if (argc > 1) return argv[1];
    return {};
#else // Linux
    std::ifstream cmdline("/proc/self/cmdline", std::ios::binary);
    std::string s;
    if (cmdline && std::getline(cmdline, s, '\0')) { // skip program name
        if (std::getline(cmdline, s, '\0')) return s;
    }
    return {};
#endif
}

inline std::filesystem::path GetExecutablePath() {
#ifdef __EMSCRIPTEN__
    // Web: executable path not available, return current directory
    return std::filesystem::current_path();
#elif _WIN32
    char buffer[MAX_PATH];
    DWORD size = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (size == 0) throw std::runtime_error("Cannot get executable path");
    return std::filesystem::path(buffer);
#elif __APPLE__
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0) {
        throw std::runtime_error("Buffer too small for executable path");
    }
    return std::filesystem::canonical(buffer);
#else // Linux
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count == -1) throw std::runtime_error("Cannot get executable path");
    return std::filesystem::path(std::string(result, count));
#endif
}

inline std::filesystem::path GetProjectRootPath() {
#ifdef __EMSCRIPTEN__
    // Web: assume assets are preloaded to /assets
    return "/assets";
#else
    if (const std::filesystem::path path = GetCommandLinePath(); !path.empty())
        return path;

    return GetExecutablePath().parent_path().append("assets");
#endif
}
