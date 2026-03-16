#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <cmath>

namespace BezierCurve {

    inline glm::vec3 EvaluateCubic(const glm::vec3& p0, const glm::vec3& p1,
                                    const glm::vec3& p2, const glm::vec3& p3, float t) {
        float u = 1.0f - t;
        return u * u * u * p0 + 3.0f * u * u * t * p1 + 3.0f * u * t * t * p2 + t * t * t * p3;
    }

    inline glm::vec3 EvaluateCubicTangent(const glm::vec3& p0, const glm::vec3& p1,
                                            const glm::vec3& p2, const glm::vec3& p3, float t) {
        float u = 1.0f - t;
        return 3.0f * u * u * (p1 - p0) + 6.0f * u * t * (p2 - p1) + 3.0f * t * t * (p3 - p2);
    }

    inline std::vector<glm::vec3> GeneratePoints(const std::vector<glm::vec3>& controlPoints, int samplesPerSegment) {
        std::vector<glm::vec3> points;
        int numSegments = (static_cast<int>(controlPoints.size()) - 1) / 3;
        for (int seg = 0; seg < numSegments; ++seg) {
            const auto& p0 = controlPoints[seg * 3];
            const auto& p1 = controlPoints[seg * 3 + 1];
            const auto& p2 = controlPoints[seg * 3 + 2];
            const auto& p3 = controlPoints[seg * 3 + 3];
            int samples = (seg == numSegments - 1) ? samplesPerSegment + 1 : samplesPerSegment;
            for (int i = 0; i < samples; ++i) {
                float t = static_cast<float>(i) / static_cast<float>(samplesPerSegment);
                points.push_back(EvaluateCubic(p0, p1, p2, p3, t));
            }
        }
        return points;
    }

    inline std::vector<glm::vec3> GenerateTangents(const std::vector<glm::vec3>& controlPoints, int samplesPerSegment) {
        std::vector<glm::vec3> tangents;
        int numSegments = (static_cast<int>(controlPoints.size()) - 1) / 3;
        for (int seg = 0; seg < numSegments; ++seg) {
            const auto& p0 = controlPoints[seg * 3];
            const auto& p1 = controlPoints[seg * 3 + 1];
            const auto& p2 = controlPoints[seg * 3 + 2];
            const auto& p3 = controlPoints[seg * 3 + 3];
            int samples = (seg == numSegments - 1) ? samplesPerSegment + 1 : samplesPerSegment;
            for (int i = 0; i < samples; ++i) {
                float t = static_cast<float>(i) / static_cast<float>(samplesPerSegment);
                glm::vec3 tan = EvaluateCubicTangent(p0, p1, p2, p3, t);
                float len = glm::length(tan);
                if (len > 1e-6f) tan /= len;
                tangents.push_back(tan);
            }
        }
        return tangents;
    }

}
