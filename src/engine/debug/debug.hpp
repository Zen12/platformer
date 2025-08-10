#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <utility>
#include "../render/line.hpp"

#ifndef NDEBUG
#define PROFILE_SCOPE(name) ScopedTimer timer(name)
#else
#define PROFILE_SCOPE(name)
#endif

#ifndef NDEBUG

struct ScopedTimer {
    std::string name;
    std::chrono::high_resolution_clock::time_point start;

    explicit ScopedTimer(std::string  n)
        : name(std::move(n)), start(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        const auto end = std::chrono::high_resolution_clock::now();
        const double duration = std::chrono::duration<double>(end - start).count(); // seconds as double
        std::cout << name << " took " << duration << " seconds\n";
    }
};


struct DebugLines {
public:
    static void AddLine(const glm::vec3& start, const glm::vec3& end);

    static void DrawLines();

    static void Clear();
};

#endif

