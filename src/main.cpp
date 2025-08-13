#include "engine/engine.hpp"

#include <iostream>
#include <filesystem>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

std::filesystem::path getExecutablePath() {
#ifdef _WIN32
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



void RunEngine() {
    std::filesystem::path path = getExecutablePath().parent_path();
    path.append("assets/");
    std::string projectRoot = path.string();

#ifndef NDEBUG
#ifdef TEST_ASSET_PATH
    projectRoot = TEST_ASSET_PATH;
#endif
#endif

    std::cout << "Load project from: " << projectRoot << "\n";
    Engine engine(projectRoot);

    engine.LoadFirstScene();

    while (engine.IsTickable())
    {
        engine.Tick();
    }

    if (engine.IsReloadRequested()) {
        RunEngine();
    }
}

int main() {

    RunEngine();
}
