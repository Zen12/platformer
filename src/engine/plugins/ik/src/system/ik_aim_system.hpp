#pragma once
#include "esc/esc_core.hpp"
#include "../component/ik_aim_component.hpp"
#include "plugins/core/src/skinned_mesh_renderer/skinned_mesh_renderer_component.hpp"
#include "plugins/core/src/transform/transform_component.hpp"
#include "plugins/core/src/time/time_component.hpp"
#include "plugins/core/src/camera/camera_component.hpp"
#include "plugins/core/src/window/window_component.hpp"
#include "plugins/core/src/navmesh_agent/navmesh_agent_component.hpp"
#include "scene.hpp"
#include "input_system.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class IKAimSystem final : public ISystemView<IKAimComponent, SkinnedMeshRendererComponent, TransformComponentV2> {
private:
    using TypeDeltaTime = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());
    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2, TransformComponentV2>());
    using TypeWindow = decltype(std::declval<entt::registry>().view<WindowComponent>());

    const TypeDeltaTime _deltaTimeView;
    const TypeCamera _cameraView;
    const TypeWindow _windowView;
    const std::weak_ptr<Scene> _scene;
    entt::registry& _registry;

    struct TwoBoneIKResult {
        glm::quat UpperRotation;
        glm::quat LowerRotation;
        bool Solved = false;
    };

    static TwoBoneIKResult SolveTwoBoneIK(
        const glm::vec3& upperPos,
        const glm::vec3& lowerPos,
        const glm::vec3& endPos,
        const glm::vec3& targetPos,
        const glm::vec3& poleVector,
        float upperLength,
        float lowerLength
    );

    static void ApplyTorsoRotation(
        std::vector<glm::mat4>& localTransforms,
        float yaw,
        float pitch,
        int spineIndex,
        int spine1Index,
        int chestIndex,
        float spineWeight,
        float spine1Weight,
        float chestWeight
    );

    static void RecomputeWorldTransforms(
        std::vector<glm::mat4>& localTransforms,
        std::vector<glm::mat4>& worldTransforms,
        const std::vector<int>& boneParents
    );

    glm::vec3 ScreenToWorldRay(const glm::vec2& screenPos, const glm::mat4& view, const glm::mat4& projection, const glm::u16vec2& windowSize) const;

public:
    explicit IKAimSystem(
        const TypeView& view,
        const TypeDeltaTime& deltaTimeView,
        const TypeCamera& cameraView,
        const TypeWindow& windowView,
        const std::weak_ptr<Scene>& scene,
        entt::registry& registry
    );

    ~IKAimSystem() override = default;

    void OnTick() override;
};
