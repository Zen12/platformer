#pragma once
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

enum class PrimitiveType {
    Triangles = GL_TRIANGLES,
    Lines = GL_LINES
};

struct RenderData final {
    const std::string MaterialGuid;
    const std::string MeshGuid;
    const glm::mat4 ModelMatrix;
    const glm::mat4 ViewMatrix;
    const glm::mat4 ProjectionMatrix;
    const std::optional<std::vector<glm::mat4>> BoneTransforms;
    const PrimitiveType Primitive = PrimitiveType::Triangles;
    const std::optional<std::vector<float>> Vertices = std::nullopt;  // Direct vertex data for lines (positions only: x,y,z,x,y,z,...)
};

struct RenderId final {
    const std::string MaterialGuid;
    const std::string MeshGuid;
    const glm::mat4 CameraView;
    const glm::mat4 CameraProjection;
    const PrimitiveType Primitive;

    RenderId(std::string material, std::string mesh, const glm::mat4 &cameraView, const glm::mat4 &cameraProjection, PrimitiveType primitive = PrimitiveType::Triangles)
        :
    MaterialGuid(std::move(material)),
    MeshGuid(std::move(mesh)),
    CameraView(cameraView),
    CameraProjection(cameraProjection),
    Primitive(primitive) {}

    bool operator==(const RenderId &other) const {
        return MaterialGuid == other.MaterialGuid && MeshGuid == other.MeshGuid && Primitive == other.Primitive;
    }

    bool operator!=(const RenderId &other) const {
        return !(*this == other);
    }
};

namespace std {
    template<>
    struct hash<RenderId> {
        std::size_t operator()(const RenderId& id) const noexcept {
            std::size_t seed = 0;

            // Hash MaterialGuid
            HashCombine(seed, std::hash<std::string>{}(id.MaterialGuid));

            // Hash MeshGuid
            HashCombine(seed, std::hash<std::string>{}(id.MeshGuid));

            // Hash Primitive type
            HashCombine(seed, std::hash<int>{}(static_cast<int>(id.Primitive)));

            // Hash CameraView matrix
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    HashCombine(seed, std::hash<float>{}(id.CameraView[i][j]));
                }
            }

            // Hash CameraProjection matrix
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    HashCombine(seed, std::hash<float>{}(id.CameraProjection[i][j]));
                }
            }

            return seed;
        }

    private:
        static void HashCombine(std::size_t& seed, const std::size_t hash) noexcept {
            // Based on boost::hash_combine
            seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };
}

struct InstanceData final {
    glm::mat4 ModelMatrix;
    std::optional<std::vector<glm::mat4>> BoneTransforms;
    std::optional<std::vector<float>> Vertices;
};

class RenderRepository final {
private:
    std::unordered_map<RenderId, std::vector<InstanceData>> _renderData{};

public:
    void Add(const RenderData &renderData) noexcept {
        const auto &id = RenderId(renderData.MaterialGuid, renderData.MeshGuid, renderData.ViewMatrix, renderData.ProjectionMatrix, renderData.Primitive);
        if (_renderData.find(id) == _renderData.end()) {
            _renderData[id] = std::vector{InstanceData{renderData.ModelMatrix, renderData.BoneTransforms, renderData.Vertices}};
            return;
        }

        _renderData[id].push_back(InstanceData{renderData.ModelMatrix, renderData.BoneTransforms, renderData.Vertices});
    }

    [[nodiscard]] const std::unordered_map<RenderId, std::vector<InstanceData>> &GetData() const noexcept {
        return _renderData;
    }

    void Clear() {
        _renderData.clear();
    }

};
