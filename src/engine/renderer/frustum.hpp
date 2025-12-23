#pragma once
#include <glm/glm.hpp>
#include <array>

class Frustum final {
public:
    enum Plane {
        Left = 0,
        Right,
        Bottom,
        Top,
        Near,
        Far
    };

private:
    std::array<glm::vec4, 6> _planes{};

public:
    void ExtractPlanes(const glm::mat4& viewProjection) noexcept {
        // Extract frustum planes from view-projection matrix
        // Based on Gribb-Hartmann method

        // Left plane
        _planes[Left].x = viewProjection[0][3] + viewProjection[0][0];
        _planes[Left].y = viewProjection[1][3] + viewProjection[1][0];
        _planes[Left].z = viewProjection[2][3] + viewProjection[2][0];
        _planes[Left].w = viewProjection[3][3] + viewProjection[3][0];

        // Right plane
        _planes[Right].x = viewProjection[0][3] - viewProjection[0][0];
        _planes[Right].y = viewProjection[1][3] - viewProjection[1][0];
        _planes[Right].z = viewProjection[2][3] - viewProjection[2][0];
        _planes[Right].w = viewProjection[3][3] - viewProjection[3][0];

        // Bottom plane
        _planes[Bottom].x = viewProjection[0][3] + viewProjection[0][1];
        _planes[Bottom].y = viewProjection[1][3] + viewProjection[1][1];
        _planes[Bottom].z = viewProjection[2][3] + viewProjection[2][1];
        _planes[Bottom].w = viewProjection[3][3] + viewProjection[3][1];

        // Top plane
        _planes[Top].x = viewProjection[0][3] - viewProjection[0][1];
        _planes[Top].y = viewProjection[1][3] - viewProjection[1][1];
        _planes[Top].z = viewProjection[2][3] - viewProjection[2][1];
        _planes[Top].w = viewProjection[3][3] - viewProjection[3][1];

        // Near plane
        _planes[Near].x = viewProjection[0][3] + viewProjection[0][2];
        _planes[Near].y = viewProjection[1][3] + viewProjection[1][2];
        _planes[Near].z = viewProjection[2][3] + viewProjection[2][2];
        _planes[Near].w = viewProjection[3][3] + viewProjection[3][2];

        // Far plane
        _planes[Far].x = viewProjection[0][3] - viewProjection[0][2];
        _planes[Far].y = viewProjection[1][3] - viewProjection[1][2];
        _planes[Far].z = viewProjection[2][3] - viewProjection[2][2];
        _planes[Far].w = viewProjection[3][3] - viewProjection[3][2];

        // Normalize all planes
        for (auto& plane : _planes) {
            const float length = glm::length(glm::vec3(plane));
            plane /= length;
        }
    }

    [[nodiscard]] bool IsBoxVisible(const glm::vec3& center, const glm::vec3& extents) const noexcept {
        // Test bounding box against all 6 frustum planes
        for (const auto& plane : _planes) {
            const glm::vec3 planeNormal(plane.x, plane.y, plane.z);
            const float planeDistance = plane.w;

            // Calculate the projection interval radius
            const float r = extents.x * glm::abs(planeNormal.x) +
                           extents.y * glm::abs(planeNormal.y) +
                           extents.z * glm::abs(planeNormal.z);

            // Distance from center to plane
            const float distance = glm::dot(planeNormal, center) + planeDistance;

            // If the box is completely outside this plane, it's not visible
            if (distance < -r) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] bool IsSphereVisible(const glm::vec3& center, float radius) const noexcept {
        // Test sphere against all 6 frustum planes
        for (const auto& plane : _planes) {
            const glm::vec3 planeNormal(plane.x, plane.y, plane.z);
            const float planeDistance = plane.w;

            const float distance = glm::dot(planeNormal, center) + planeDistance;

            // If sphere is completely outside this plane, it's not visible
            if (distance < -radius) {
                return false;
            }
        }

        return true;
    }
};
