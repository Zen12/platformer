#pragma once
#include <glm/glm.hpp>
#include <limits>

struct Bounds {
    glm::vec3 Center{0.0f};
    glm::vec3 Extents{0.0f};  // Half-size in each dimension

    Bounds() = default;

    Bounds(const glm::vec3& center, const glm::vec3& extents)
        : Center(center), Extents(extents) {}

    // Create bounds from min/max points
    static Bounds FromMinMax(const glm::vec3& min, const glm::vec3& max) {
        Bounds b;
        b.Center = (min + max) * 0.5f;
        b.Extents = (max - min) * 0.5f;
        return b;
    }

    [[nodiscard]] glm::vec3 GetMin() const noexcept {
        return Center - Extents;
    }

    [[nodiscard]] glm::vec3 GetMax() const noexcept {
        return Center + Extents;
    }

    // Transform bounds by a model matrix (returns axis-aligned bounds in world space)
    [[nodiscard]] Bounds Transform(const glm::mat4& model) const noexcept {
        // Get world-space center
        glm::vec3 worldCenter = glm::vec3(model * glm::vec4(Center, 1.0f));

        // For AABB, we need to compute new extents based on the absolute values
        // of the rotated axes scaled by original extents
        glm::vec3 worldExtents{0.0f};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                worldExtents[i] += glm::abs(model[j][i]) * Extents[j];
            }
        }

        return {worldCenter, worldExtents};
    }

    [[nodiscard]] bool IsValid() const noexcept {
        return Extents.x > 0.0f || Extents.y > 0.0f || Extents.z > 0.0f;
    }
};
