#pragma once
#include "../esc_core.hpp"
#include "simple_animation_component.hpp"
#include "../skinned_mesh_renderer/skinned_mesh_renderer_component.hpp"
#include "../transform/transform_component.hpp"
#include "../time/time_component.hpp"
#include "../../renderer/animation/animation_data.hpp"
#include "../../scene/scene.hpp"
#include "guid.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>
#include <unordered_map>

#define DEBUG_SIMPLE_ANIMATION 0

#if DEBUG_SIMPLE_ANIMATION
#include <iostream>
#define ANIM_SYS_LOG if(1) std::cout
#else
#define ANIM_SYS_LOG if(0) std::cout
#endif

class SimpleAnimationSystem final : public ISystemView<SimpleAnimationComponent, SkinnedMeshRendererComponent, TransformComponentV2> {
private:
    using TypeDeltaTime = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());
    const TypeDeltaTime _deltaTimeView;
    const std::weak_ptr<Scene> _scene;
    std::unordered_map<Guid, std::shared_ptr<AnimationData>> _loadedAnimations;

    static std::string TryMatchBoneName(const std::string& animBoneName, const std::unordered_map<std::string, int>& boneMap) {
        if (boneMap.find(animBoneName) != boneMap.end()) {
            return animBoneName;
        }
        return "";
    }

public:
    explicit SimpleAnimationSystem(const TypeView &view, const TypeDeltaTime &deltaTimeView, const std::weak_ptr<Scene> &scene)
        : ISystemView(view), _deltaTimeView(deltaTimeView), _scene(scene) {
    }

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

    void OnTick() override {
        float deltaTime = 0.016f;

        for (const auto &[_, dt] : _deltaTimeView.each()) {
            deltaTime = dt.Delta;
        }

        auto scene = _scene.lock();
        if (!scene) return;

        for (const auto &[_, anim, skinnedMesh, transform] : View.each()) {
            if (!anim.Enabled || anim.AnimationGuid.IsEmpty()) continue;

            if (skinnedMesh.BoneNames.empty()) {
                scene->GetMesh(skinnedMesh.MeshGuid);

                skinnedMesh.BoneNames = scene->GetMeshBoneNames(skinnedMesh.MeshGuid);
                skinnedMesh.BoneOffsets = scene->GetMeshBoneOffsets(skinnedMesh.MeshGuid);
                skinnedMesh.BoneParents = scene->GetMeshBoneParents(skinnedMesh.MeshGuid);
                if (!skinnedMesh.BoneNames.empty()) {
                    ANIM_SYS_LOG << "Loaded " << skinnedMesh.BoneNames.size() << " bones with hierarchy for mesh" << std::endl;
                }
            }

            if (_loadedAnimations.find(anim.AnimationGuid) == _loadedAnimations.end()) {
                auto animData = scene->GetAnimation(anim.AnimationGuid);
                if (animData) {
                    _loadedAnimations[anim.AnimationGuid] = animData;
                    ANIM_SYS_LOG << "Loaded animation: " << animData->Name << " (" << animData->Duration << "s)" << std::endl;
                } else {
                    ANIM_SYS_LOG << "Failed to load animation: " << anim.AnimationGuid.ToString() << std::endl;
                    anim.Enabled = false;
                    continue;
                }
            }

            auto animData = _loadedAnimations[anim.AnimationGuid];
            if (!animData) continue;

            anim.Time += deltaTime;
            if (anim.Loop && anim.Time > animData->Duration) {
                anim.Time = fmod(anim.Time, animData->Duration);
            }

            std::unordered_map<std::string, int> boneNameToIndex;
            for (size_t i = 0; i < skinnedMesh.BoneNames.size(); i++) {
                boneNameToIndex[skinnedMesh.BoneNames[i]] = static_cast<int>(i);
            }

            if (skinnedMesh.LocalBoneTransforms.size() != skinnedMesh.BoneNames.size()) {
                skinnedMesh.LocalBoneTransforms.resize(skinnedMesh.BoneNames.size(), glm::mat4(1.0f));
            }

            std::vector<glm::mat4>& localTransforms = skinnedMesh.LocalBoneTransforms;

            for (const auto& channel : animData->Channels) {
                std::string meshBoneName = TryMatchBoneName(channel.BoneName, boneNameToIndex);
                if (meshBoneName.empty()) {
                    continue;
                }

                const int boneIndex = boneNameToIndex[meshBoneName];

                glm::vec3 position = SamplePosition(channel.PositionKeys, anim.Time);
                glm::quat rotation = SampleRotation(channel.RotationKeys, anim.Time);
                glm::vec3 scale = SampleScale(channel.ScaleKeys, anim.Time);

                glm::mat4 localTransform = glm::mat4(1.0f);
                localTransform = glm::translate(localTransform, position);
                localTransform = localTransform * glm::toMat4(rotation);
                localTransform = glm::scale(localTransform, scale);

                localTransforms[boneIndex] = localTransform;
            }

            std::vector<glm::mat4> worldTransforms(skinnedMesh.BoneNames.size(), glm::mat4(1.0f));
            std::vector<bool> processed(skinnedMesh.BoneNames.size(), false);

            const auto& boneParents = skinnedMesh.BoneParents;

            std::function<void(int)> computeWorldTransform = [&](int boneIndex) {
                if (processed[boneIndex]) return;

                int parentIndex = boneParents[boneIndex];
                if (parentIndex == -1) {
                    worldTransforms[boneIndex] = localTransforms[boneIndex];
                } else {
                    computeWorldTransform(parentIndex);
                    worldTransforms[boneIndex] = worldTransforms[parentIndex] * localTransforms[boneIndex];
                }
                processed[boneIndex] = true;
            };

            for (size_t i = 0; i < skinnedMesh.BoneNames.size(); i++) {
                computeWorldTransform(static_cast<int>(i));
            }

            for (size_t i = 0; i < skinnedMesh.BoneTransforms.size(); i++) {
                if (i < worldTransforms.size() && i < skinnedMesh.BoneOffsets.size()) {
                    skinnedMesh.BoneTransforms[i] = worldTransforms[i] * skinnedMesh.BoneOffsets[i];
                } else {
                    skinnedMesh.BoneTransforms[i] = glm::mat4(1.0f);
                }
            }
        }
    }
};
