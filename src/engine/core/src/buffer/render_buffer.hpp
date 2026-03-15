#pragma once
#include <array>
#include <unordered_map>
#include <utility>
#include <vector>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "guid.hpp"

enum class PrimitiveType {
    Triangles = 0,
    Lines = 1
};

struct RenderData final {
    const Guid MaterialGuid;
    const Guid MeshGuid;
    const glm::mat4 ModelMatrix;
    const glm::mat4 ViewMatrix;
    const glm::mat4 ProjectionMatrix;
    const std::optional<std::vector<glm::mat4>> BoneTransforms;
    const PrimitiveType Primitive = PrimitiveType::Triangles;
    const std::optional<std::vector<glm::vec3>> Positions = std::nullopt;
    const std::optional<glm::vec4> LineColor = std::nullopt;
    const std::optional<glm::vec4> InstanceColor = std::nullopt;
    const bool IsSkinned = false;
    const float YDepthFactor = 0.0f;  // Y-based depth offset for 2.5D sorting (0 = disabled)
};

struct RenderId final {
    const Guid MaterialGuid;
    const Guid MeshGuid;
    const glm::mat4 CameraView;
    const glm::mat4 CameraProjection;
    const PrimitiveType Primitive;
    const bool IsSkinned;
    const float YDepthFactor;  // Y-based depth offset for 2.5D sorting (0 = disabled)

    RenderId(const Guid& material, const Guid& mesh, const glm::mat4 &cameraView, const glm::mat4 &cameraProjection, PrimitiveType primitive = PrimitiveType::Triangles, bool isSkinned = false, float yDepthFactor = 0.0f)
        :
    MaterialGuid(material),
    MeshGuid(mesh),
    CameraView(cameraView),
    CameraProjection(cameraProjection),
    Primitive(primitive),
    IsSkinned(isSkinned),
    YDepthFactor(yDepthFactor) {}

    bool operator==(const RenderId &other) const {
        return MaterialGuid == other.MaterialGuid && MeshGuid == other.MeshGuid && Primitive == other.Primitive && IsSkinned == other.IsSkinned;
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

            HashCombine(seed, std::hash<Guid>{}(id.MaterialGuid));
            HashCombine(seed, std::hash<Guid>{}(id.MeshGuid));
            HashCombine(seed, std::hash<int>{}(static_cast<int>(id.Primitive)));
            HashCombine(seed, std::hash<bool>{}(id.IsSkinned));

            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    HashCombine(seed, std::hash<float>{}(id.CameraView[i][j]));
                }
            }

            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    HashCombine(seed, std::hash<float>{}(id.CameraProjection[i][j]));
                }
            }

            return seed;
        }

    private:
        static void HashCombine(std::size_t& seed, const std::size_t hash) noexcept {
            seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };
}

struct InstanceData final {
    glm::mat4 ModelMatrix;
    std::optional<std::vector<glm::mat4>> BoneTransforms;
    std::optional<std::vector<glm::vec3>> Positions;
    std::optional<glm::vec4> LineColor;
    std::optional<glm::vec4> InstanceColor;
};

struct DirectionalLightData final {
    glm::mat4 View{1.0f};
    glm::mat4 Projection{1.0f};
    bool HasLight{false};
};

struct SpotLightData final {
    glm::vec3 Position{0.0f};
    glm::vec3 Direction{0.0f, -1.0f, 0.0f};
    glm::vec3 Color{1.0f};
    float InnerCutoff{0.9f};
    float OuterCutoff{0.8f};
    float Range{50.0f};
    float Intensity{1.0f};
    glm::mat4 View{1.0f};
    glm::mat4 Projection{1.0f};
    bool HasLight{false};
};

static constexpr int MAX_SPOT_LIGHTS = 4;

class RenderBuffer final {
private:
    std::unordered_map<RenderId, std::vector<InstanceData>> _renderData{};
    DirectionalLightData _directionalLight{};
    std::array<SpotLightData, MAX_SPOT_LIGHTS> _spotLights{};
    int _numSpotLights{0};

public:
    void SetDirectionalLight(const glm::mat4& view, const glm::mat4& projection) noexcept {
        _directionalLight.View = view;
        _directionalLight.Projection = projection;
        _directionalLight.HasLight = true;
    }

    [[nodiscard]] const DirectionalLightData& GetDirectionalLight() const noexcept {
        return _directionalLight;
    }

    void AddSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color,
                      float innerCutoff, float outerCutoff, float range, float intensity,
                      const glm::mat4& view, const glm::mat4& projection) noexcept {
        if (_numSpotLights >= MAX_SPOT_LIGHTS) return;
        auto& sl = _spotLights[_numSpotLights];
        sl.Position = position;
        sl.Direction = direction;
        sl.Color = color;
        sl.InnerCutoff = innerCutoff;
        sl.OuterCutoff = outerCutoff;
        sl.Range = range;
        sl.Intensity = intensity;
        sl.View = view;
        sl.Projection = projection;
        sl.HasLight = true;
        _numSpotLights++;
    }

    [[nodiscard]] const std::array<SpotLightData, MAX_SPOT_LIGHTS>& GetSpotLights() const noexcept {
        return _spotLights;
    }

    [[nodiscard]] int GetNumSpotLights() const noexcept {
        return _numSpotLights;
    }

    void Add(const RenderData &renderData) noexcept {
        const auto &id = RenderId(renderData.MaterialGuid, renderData.MeshGuid, renderData.ViewMatrix, renderData.ProjectionMatrix, renderData.Primitive, renderData.IsSkinned, renderData.YDepthFactor);
        if (_renderData.find(id) == _renderData.end()) {
            _renderData[id] = std::vector{InstanceData{renderData.ModelMatrix, renderData.BoneTransforms, renderData.Positions, renderData.LineColor, renderData.InstanceColor}};
            return;
        }

        _renderData[id].push_back(InstanceData{renderData.ModelMatrix, renderData.BoneTransforms, renderData.Positions, renderData.LineColor, renderData.InstanceColor});
    }

    [[nodiscard]] const std::unordered_map<RenderId, std::vector<InstanceData>> &GetData() const noexcept {
        return _renderData;
    }

    void Clear() {
        _renderData.clear();
        _directionalLight = DirectionalLightData{};
        _spotLights = {};
        _numSpotLights = 0;
    }

};
