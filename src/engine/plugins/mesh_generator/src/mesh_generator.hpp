#pragma once
#include <vector>
#include <variant>
#include <memory>
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <manifold/manifold.h>
#include "meshgen_asset.hpp"
#include "meshgen_asset_yaml.hpp"
#include "bezier_asset.hpp"
#include "bezier_asset_yaml.hpp"
#include "bezier_curve.hpp"
#include "bounds.hpp"
#include "asset_manager.hpp"

struct GeneratedMeshData {
    std::vector<float> Vertices;
    std::vector<uint32_t> Indices;
    Bounds MeshBounds;
    bool HasVertexColor = false;
};

namespace MeshGenerator {

    inline manifold::vec3 ToManifold(const glm::vec3& v) {
        return {static_cast<double>(v.x), static_cast<double>(v.y), static_cast<double>(v.z)};
    }

    inline manifold::Manifold CreatePrimitive(const MeshGenOperation& op) {
        manifold::Manifold result;
        if (op.Type == "cube") {
            result = manifold::Manifold::Cube(ToManifold(op.Size), true);
        } else if (op.Type == "sphere") {
            result = manifold::Manifold::Sphere(static_cast<double>(op.Radius), op.Segments);
        } else if (op.Type == "cylinder") {
            result = manifold::Manifold::Cylinder(
                static_cast<double>(op.Height),
                static_cast<double>(op.Radius),
                static_cast<double>(op.Radius),
                op.Segments, true);
        }
        return result;
    }

    inline manifold::Manifold EnrichWithColor(const manifold::Manifold& m, const glm::vec3& color) {
        auto srcGL = m.GetMeshGL();
        const int srcNumProp = srcGL.numProp;
        const int numVerts = static_cast<int>(srcGL.vertProperties.size()) / srcNumProp;

        manifold::MeshGL dstGL;
        dstGL.numProp = 6;
        dstGL.vertProperties.resize(numVerts * 6);

        for (int i = 0; i < numVerts; ++i) {
            dstGL.vertProperties[i * 6 + 0] = srcGL.vertProperties[i * srcNumProp + 0];
            dstGL.vertProperties[i * 6 + 1] = srcGL.vertProperties[i * srcNumProp + 1];
            dstGL.vertProperties[i * 6 + 2] = srcGL.vertProperties[i * srcNumProp + 2];
            dstGL.vertProperties[i * 6 + 3] = color.r;
            dstGL.vertProperties[i * 6 + 4] = color.g;
            dstGL.vertProperties[i * 6 + 5] = color.b;
        }

        dstGL.triVerts = srcGL.triVerts;

        auto result = manifold::Manifold(dstGL);
        if (result.Status() != manifold::Manifold::Error::NoError) {
            std::cerr << "[MeshGen] EnrichWithColor manifold error: "
                      << static_cast<int>(result.Status()) << std::endl;
        }
        return result;
    }

    inline manifold::Manifold ApplyTransform(manifold::Manifold m, const MeshGenOperation& op) {
        if (op.Scale != glm::vec3(1.0f)) {
            m = m.Scale(ToManifold(op.Scale));
        }
        if (op.Rotate != glm::vec3(0.0f)) {
            m = m.Rotate(static_cast<double>(op.Rotate.x),
                         static_cast<double>(op.Rotate.y),
                         static_cast<double>(op.Rotate.z));
        }
        if (op.Translate != glm::vec3(0.0f)) {
            m = m.Translate(ToManifold(op.Translate));
        }
        return m;
    }

    inline manifold::Manifold BuildBezierTube(const MeshGenOperation& op,
                                               const std::weak_ptr<AssetManager>& assetManager) {
        auto am = assetManager.lock();
        if (!am) return {};

        const auto bezier = am->LoadAssetByGuid<BezierAsset>(op.BezierGuid);
        if (bezier.ControlPoints.size() < 4) return {};

        int samplesPerSegment = std::max(op.Segments, 4);
        auto pathPoints = BezierCurve::GeneratePoints(bezier.ControlPoints, samplesPerSegment);
        auto pathTangents = BezierCurve::GenerateTangents(bezier.ControlPoints, samplesPerSegment);

        float tubeRadius = op.Height * 0.5f;
        int circleSeg = std::max(op.Segments / 2, 6);
        int numPath = static_cast<int>(pathPoints.size());

        const float r = op.Color.r;
        const float g = op.Color.g;
        const float b = op.Color.b;

        std::vector<float> verts;
        std::vector<uint32_t> tris;

        for (int i = 0; i < numPath; ++i) {
            glm::vec3 T = pathTangents[i];
            if (glm::length(T) < 1e-6f) T = glm::vec3(0.0f, 0.0f, 1.0f);
            T = glm::normalize(T);

            glm::vec3 up = (std::abs(glm::dot(T, glm::vec3(0, 1, 0))) > 0.99f)
                           ? glm::vec3(1, 0, 0)
                           : glm::vec3(0, 1, 0);
            glm::vec3 N = glm::normalize(glm::cross(T, up));
            glm::vec3 B = glm::cross(T, N);

            for (int j = 0; j < circleSeg; ++j) {
                float angle = 2.0f * 3.14159265f * static_cast<float>(j) / static_cast<float>(circleSeg);
                glm::vec3 pos = pathPoints[i] + tubeRadius * (std::cos(angle) * N + std::sin(angle) * B);
                verts.push_back(pos.x);
                verts.push_back(pos.y);
                verts.push_back(pos.z);
                verts.push_back(r);
                verts.push_back(g);
                verts.push_back(b);
            }
        }

        for (int i = 0; i < numPath - 1; ++i) {
            for (int j = 0; j < circleSeg; ++j) {
                uint32_t c  = static_cast<uint32_t>(i * circleSeg + j);
                uint32_t cn = static_cast<uint32_t>((i + 1) * circleSeg + j);
                uint32_t n  = static_cast<uint32_t>(i * circleSeg + (j + 1) % circleSeg);
                uint32_t nn = static_cast<uint32_t>((i + 1) * circleSeg + (j + 1) % circleSeg);

                tris.push_back(c);  tris.push_back(n);  tris.push_back(cn);
                tris.push_back(n);  tris.push_back(nn); tris.push_back(cn);
            }
        }

        uint32_t startC = static_cast<uint32_t>(verts.size() / 6);
        verts.push_back(pathPoints[0].x);
        verts.push_back(pathPoints[0].y);
        verts.push_back(pathPoints[0].z);
        verts.push_back(r); verts.push_back(g); verts.push_back(b);
        for (int j = 0; j < circleSeg; ++j) {
            uint32_t j0 = static_cast<uint32_t>(j);
            uint32_t j1 = static_cast<uint32_t>((j + 1) % circleSeg);
            tris.push_back(startC); tris.push_back(j1); tris.push_back(j0);
        }

        uint32_t endC = static_cast<uint32_t>(verts.size() / 6);
        verts.push_back(pathPoints[numPath - 1].x);
        verts.push_back(pathPoints[numPath - 1].y);
        verts.push_back(pathPoints[numPath - 1].z);
        verts.push_back(r); verts.push_back(g); verts.push_back(b);
        uint32_t endRing = static_cast<uint32_t>((numPath - 1) * circleSeg);
        for (int j = 0; j < circleSeg; ++j) {
            uint32_t j0 = endRing + static_cast<uint32_t>(j);
            uint32_t j1 = endRing + static_cast<uint32_t>((j + 1) % circleSeg);
            tris.push_back(endC); tris.push_back(j0); tris.push_back(j1);
        }

        manifold::MeshGL meshGL;
        meshGL.numProp = 6;
        meshGL.vertProperties = std::move(verts);
        meshGL.triVerts = std::move(tris);

        auto result = manifold::Manifold(meshGL);
        if (result.Status() != manifold::Manifold::Error::NoError) {
            std::cerr << "[MeshGen] Bezier tube manifold error: "
                      << static_cast<int>(result.Status()) << std::endl;
        }
        return result;
    }

    inline manifold::Manifold BuildResultManifold(const MeshGenAsset& asset,
                                                    const std::weak_ptr<AssetManager>& assetManager) {
        std::vector<manifold::Manifold> results;
        results.reserve(asset.Operations.size());

        for (const auto& op : asset.Operations) {
            if (op.Type == "cube" || op.Type == "sphere" || op.Type == "cylinder") {
                auto prim = CreatePrimitive(op);
                prim = EnrichWithColor(prim, op.Color);
                prim = ApplyTransform(std::move(prim), op);
                results.push_back(std::move(prim));
            } else if (op.Type == "bezier_extrude") {
                auto tube = BuildBezierTube(op, assetManager);
                tube = ApplyTransform(std::move(tube), op);
                results.push_back(std::move(tube));
            } else if (op.Type == "custom" || op.Type == "custome") {
                auto am = assetManager.lock();
                if (!am) continue;
                const auto customAsset = am->LoadAssetByGuid<MeshGenAsset>(op.MeshGuid);
                auto customManifold = BuildResultManifold(customAsset, assetManager);
                customManifold = ApplyTransform(std::move(customManifold), op);
                results.push_back(std::move(customManifold));
            } else if ((op.Type == "union" || op.Type == "difference" || op.Type == "intersection")
                       && op.Operands.size() >= 2) {
                auto combined = results[op.Operands[0]];
                for (size_t i = 1; i < op.Operands.size(); ++i) {
                    if (op.Type == "union") combined = combined + results[op.Operands[i]];
                    else if (op.Type == "difference") combined = combined - results[op.Operands[i]];
                    else combined = combined ^ results[op.Operands[i]];
                }
                results.push_back(std::move(combined));
            }
        }

        return results.empty() ? manifold::Manifold{} : results.back();
    }

    inline GeneratedMeshData Generate(const MeshGenAsset& asset,
                                       const std::weak_ptr<AssetManager>& assetManager) {
        const auto finalManifold = BuildResultManifold(asset, assetManager);

        if (finalManifold.IsEmpty()) return {};

        auto meshGL = finalManifold.GetMeshGL();

        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        const int numVerts = static_cast<int>(meshGL.vertProperties.size()) / meshGL.numProp;
        vertices.reserve(numVerts * 6);

        glm::vec3 minBounds(std::numeric_limits<float>::max());
        glm::vec3 maxBounds(std::numeric_limits<float>::lowest());

        for (int i = 0; i < numVerts; ++i) {
            const float x = meshGL.vertProperties[i * meshGL.numProp + 0];
            const float y = meshGL.vertProperties[i * meshGL.numProp + 1];
            const float z = meshGL.vertProperties[i * meshGL.numProp + 2];

            float r = 1.0f, g = 1.0f, b = 1.0f;
            if (meshGL.numProp >= 6) {
                r = meshGL.vertProperties[i * meshGL.numProp + 3];
                g = meshGL.vertProperties[i * meshGL.numProp + 4];
                b = meshGL.vertProperties[i * meshGL.numProp + 5];
            }

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);

            minBounds = glm::min(minBounds, glm::vec3(x, y, z));
            maxBounds = glm::max(maxBounds, glm::vec3(x, y, z));
        }

        indices.reserve(meshGL.triVerts.size());
        for (auto idx : meshGL.triVerts) {
            indices.push_back(static_cast<uint32_t>(idx));
        }

        return {std::move(vertices), std::move(indices), Bounds::FromMinMax(minBounds, maxBounds), true};
    }

}
