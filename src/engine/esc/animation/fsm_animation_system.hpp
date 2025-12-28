#pragma once
#include "../esc_core.hpp"
#include "fsm_animation_component.hpp"
#include "../skinned_mesh_renderer/skinned_mesh_renderer_component.hpp"
#include "../transform/transform_component.hpp"
#include "../time/time_component.hpp"
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
    std::unordered_map<std::string, std::shared_ptr<AnimationData>> _loadedAnimations;
    std::unordered_map<entt::entity, std::shared_ptr<FsmController>> _fsmControllers;
    std::unordered_map<entt::entity, std::shared_ptr<FsmAnimationComponent>> _animationComponentPtrs;

    // Helper: Try to match animation bone name to mesh bone name
    static std::string TryMatchBoneName(const std::string& animBoneName, const std::unordered_map<std::string, int>& boneMap) {
        if (boneMap.find(animBoneName) != boneMap.end()) {
            return animBoneName;
        }

        std::string boneName = animBoneName;
        const std::string mixamoPrefix = "mixamorig:";
        if (boneName.substr(0, mixamoPrefix.length()) == mixamoPrefix) {
            boneName = boneName.substr(mixamoPrefix.length());
        }

        if (boneMap.find(boneName) != boneMap.end()) {
            return boneName;
        }

        static const std::unordered_map<std::string, std::string> mixamoToRigify = {
            {"Hips", "hip"}, {"Spine", "hip"}, {"Spine1", "chest"}, {"Spine2", "chest"},
            {"Neck", "neck"}, {"Head", "head"},
            {"LeftShoulder", "KTF.L"}, {"LeftArm", "upperarm.L"}, {"LeftForeArm", "lowerarm.L"}, {"LeftHand", "hand.L"},
            {"RightShoulder", "KTF.R"}, {"RightArm", "upperarm.R"}, {"RightForeArm", "lowerarm.R"}, {"RightHand", "hand.R"},
            {"LeftUpLeg", "upperleg.L"}, {"LeftLeg", "lowerleg.L"}, {"LeftFoot", "foot.L"}, {"LeftToeBase", "toose.L"},
            {"RightUpLeg", "upperleg.R"}, {"RightLeg", "lowerleg.R"}, {"RightFoot", "foot.R"}, {"RightToeBase", "toose.R"},
            {"LeftHandThumb1", "thumb.01.L"}, {"LeftHandThumb2", "thumb.02.L"},
            {"LeftHandIndex1", "f_index.01.L"}, {"LeftHandIndex2", "f_index.02.L"},
            {"LeftHandMiddle1", "f_middle.01.L"}, {"LeftHandMiddle2", "f_middle.02.L"},
            {"LeftHandRing1", "f_ring.01.L"}, {"LeftHandRing2", "f_ring.02.L"},
            {"LeftHandPinky1", "f_pinky.01.L"}, {"LeftHandPinky2", "f_pinky.02.L"},
            {"RightHandThumb1", "thumb.01.R"}, {"RightHandThumb2", "thumb.02.R"},
            {"RightHandIndex1", "f_index.01.R"}, {"RightHandIndex2", "f_index.02.R"},
            {"RightHandMiddle1", "f_middle.01.R"}, {"RightHandMiddle2", "f_middle.02.R"},
            {"RightHandRing1", "f_ring.01.R"}, {"RightHandRing2", "f_ring.02.R"},
            {"RightHandPinky1", "f_pinky.01.R"}, {"RightHandPinky2", "f_pinky.02.R"},
        };

        auto it = mixamoToRigify.find(boneName);
        if (it != mixamoToRigify.end()) {
            const std::string& rigifyName = it->second;
            if (boneMap.find(rigifyName) != boneMap.end()) {
                return rigifyName;
            }
        }

        return "";
    }

public:
    explicit FsmAnimationSystem(const TypeView &view, const TypeDeltaTime &deltaTimeView, const std::weak_ptr<Scene> &scene)
        : ISystemView(view), _deltaTimeView(deltaTimeView), _scene(scene) {
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
