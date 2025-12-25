#pragma once
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct RenderData final {
    const std::string MaterialGuid;
    const std::string MeshGuid;
    const glm::mat4 ModelMatrix;
    const glm::mat4 ViewMatrix;
    const glm::mat4 ProjectionMatrix;
    const std::optional<std::vector<glm::mat4>> BoneTransforms;
};

struct RenderId final {
    const std::string MaterialGuid;
    const std::string MeshGuid;
    const glm::mat4 CameraView;
    const glm::mat4 CameraProjection;

    RenderId(std::string material, std::string mesh, const glm::mat4 &cameraView, const glm::mat4 &cameraProjection)
        :
    MaterialGuid(std::move(material)),
    MeshGuid(std::move(mesh)),
    CameraView(cameraView),
    CameraProjection(cameraProjection) {}

    bool operator==(const RenderId &other) const {
        return MaterialGuid == other.MaterialGuid && MeshGuid == other.MeshGuid;
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
};

class RenderRepository final {
private:
    std::unordered_map<RenderId, std::vector<InstanceData>> _renderData{};


public:
    void Add(const RenderData &renderData) noexcept {
        const auto &id = RenderId(renderData.MaterialGuid, renderData.MeshGuid, renderData.ViewMatrix, renderData.ProjectionMatrix);
        if (_renderData.find(id) == _renderData.end()) {
            _renderData[id] = std::vector{InstanceData{renderData.ModelMatrix, renderData.BoneTransforms}};
            return;
        }

        _renderData[id].push_back(InstanceData{renderData.ModelMatrix, renderData.BoneTransforms});
    }

    [[nodiscard]] const std::unordered_map<RenderId, std::vector<InstanceData>> &GetData() const noexcept {
        return _renderData;
    }

    void Clear() {
        _renderData.clear();
    }

};
