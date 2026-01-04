#pragma once
#include "../esc_core.hpp"
#include "fsm_animation_component.hpp"
#include "../skinned_mesh_renderer/skinned_mesh_renderer_component.hpp"
#include "../transform/transform_component.hpp"
#include "../time/time_component.hpp"
#include "../navmesh_agent/navmesh_agent_component.hpp"
#include "../../renderer/animation/animation_data.hpp"
#include "../../scene/scene.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <entt/entt.hpp>

// Forward declarations to avoid circular dependency
class FsmController;
struct FsmAsset;

class FsmAnimationSystem final : public ISystemView<FsmAnimationComponent, SkinnedMeshRendererComponent, TransformComponentV2> {
private:
    using TypeDeltaTime = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());
    const TypeDeltaTime _deltaTimeView;
    const std::weak_ptr<Scene> _scene;
    entt::registry& _registry;
    std::unordered_map<std::string, std::shared_ptr<AnimationData>> _loadedAnimations;

    // Helper: Try to match animation bone name to mesh bone name
    static std::string TryMatchBoneName(const std::string& animBoneName, const std::unordered_map<std::string, int>& boneMap) {
        if (boneMap.find(animBoneName) != boneMap.end()) {
            return animBoneName;
        }
        return "";
    }

public:
    explicit FsmAnimationSystem(const TypeView &view, const TypeDeltaTime &deltaTimeView, const std::weak_ptr<Scene> &scene, entt::registry& registry)
        : ISystemView(view), _deltaTimeView(deltaTimeView), _scene(scene), _registry(registry) {
    }

    ~FsmAnimationSystem() override;

    template<typename T>
    static size_t FindKeyIndex(const std::vector<T>& keys, float time) {
        for (size_t i = 0; i < keys.size() - 1; i++) {
            if (time < keys[i + 1].Time) {
                return i;
            }
        }
        return keys.size() - 2;
    }

    static glm::vec3 SamplePosition(const std::vector<PositionKey>& keys, float time) {
        if (keys.empty()) return glm::vec3(0.0f);
        if (keys.size() == 1 || time <= keys[0].Time) return keys[0].Value;
        if (time >= keys.back().Time) return keys.back().Value;

        size_t index = FindKeyIndex(keys, time);
        float t = (time - keys[index].Time) / (keys[index + 1].Time - keys[index].Time);
        return glm::mix(keys[index].Value, keys[index + 1].Value, t);
    }

    static glm::quat SampleRotation(const std::vector<RotationKey>& keys, float time) {
        if (keys.empty()) return glm::quat(1, 0, 0, 0);
        if (keys.size() == 1 || time <= keys[0].Time) return keys[0].Value;
        if (time >= keys.back().Time) return keys.back().Value;

        size_t index = FindKeyIndex(keys, time);
        float t = (time - keys[index].Time) / (keys[index + 1].Time - keys[index].Time);
        return glm::slerp(keys[index].Value, keys[index + 1].Value, t);
    }

    static glm::vec3 SampleScale(const std::vector<ScaleKey>& keys, float time) {
        if (keys.empty()) return glm::vec3(1.0f);
        if (keys.size() == 1 || time <= keys[0].Time) return keys[0].Value;
        if (time >= keys.back().Time) return keys.back().Value;

        size_t index = FindKeyIndex(keys, time);
        float t = (time - keys[index].Time) / (keys[index + 1].Time - keys[index].Time);
        return glm::mix(keys[index].Value, keys[index + 1].Value, t);
    }

    void OnTick() override;
};
