#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <utility>

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
#endif
