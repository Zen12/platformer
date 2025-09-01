#pragma once

#include <iostream>
#include <chrono>
#include <iomanip>
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
        const double durationMs = std::chrono::duration<double, std::milli>(end - start).count(); // milliseconds

        std::cout << std::left << std::setw(20) << name << " took "
                  << std::fixed << std::setprecision(2)
                  << std::setw(15) << durationMs << " ms\n";
    }
};


struct DebugLines {
public:
    static void Init();

    static void UpdateViewProjection(const glm::mat4& view, const glm::mat4& projection);

    static void AddLine(const glm::vec3& start, const glm::vec3& end);
    static void AddLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
    static void AddLine(const glm::vec2 &start, const glm::vec2 &end);

    static void DrawLines();

    static void Clear();
};

#endif

