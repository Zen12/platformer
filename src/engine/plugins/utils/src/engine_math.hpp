#pragma once

#include <random>

class EngineMath {
    public:
    [[nodiscard]] static float InverseLerp(const float &a, const float &b, const float &v) {
        if (a == b) return 0.0f; // avoid division by zero
        return (v - a) / (b - a);
    }

    [[nodiscard]] static float Lerp(const float &a, const float &b, const float &t) {
        return a + t * (b - a);
    }

    // Persistent engine and distribution
    [[nodiscard]] static std::mt19937& GetEngine() {
        static std::random_device rd;
        static std::mt19937 gen(rd()); // seeded once
        return gen;
    }

    [[nodiscard]] static float RandomFloat(const float &min = 0.0f, const float &max = 1.0f) {
        static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return min + (max - min) * dist(GetEngine());
    }
};