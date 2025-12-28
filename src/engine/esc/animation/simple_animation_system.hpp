#pragma once
#include "../esc_core.hpp"
#include "simple_animation_component.hpp"
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

class SimpleAnimationSystem final : public ISystemView<SimpleAnimationComponent, SkinnedMeshRendererComponent, TransformComponentV2> {
private:
    using TypeDeltaTime = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());
    const TypeDeltaTime _deltaTimeView;
    const std::weak_ptr<Scene> _scene;
    std::unordered_map<std::string, std::shared_ptr<AnimationData>> _loadedAnimations;

    // Helper: Try to match animation bone name to mesh bone name
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

    // Helper: Find keyframe index for sampling
    template<typename T>
    static size_t FindKeyIndex(const std::vector<T>& keys, float time) {
        for (size_t i = 0; i < keys.size() - 1; i++) {
            if (time < keys[i + 1].Time) {
                return i;
            }
        }
        return keys.size() - 2;
    }

    // Sample position at given time
    static glm::vec3 SamplePosition(const std::vector<PositionKey>& keys, float time) {
        if (keys.empty()) return glm::vec3(0.0f);
        if (keys.size() == 1 || time <= keys[0].Time) return keys[0].Value;
        if (time >= keys.back().Time) return keys.back().Value;

        size_t index = FindKeyIndex(keys, time);
        float t = (time - keys[index].Time) / (keys[index + 1].Time - keys[index].Time);
        return glm::mix(keys[index].Value, keys[index + 1].Value, t);
    }

    // Sample rotation at given time
    static glm::quat SampleRotation(const std::vector<RotationKey>& keys, float time) {
        if (keys.empty()) return glm::quat(1, 0, 0, 0);
        if (keys.size() == 1 || time <= keys[0].Time) return keys[0].Value;
        if (time >= keys.back().Time) return keys.back().Value;

        size_t index = FindKeyIndex(keys, time);
        float t = (time - keys[index].Time) / (keys[index + 1].Time - keys[index].Time);
        return glm::slerp(keys[index].Value, keys[index + 1].Value, t);
    }

    // Sample scale at given time
    static glm::vec3 SampleScale(const std::vector<ScaleKey>& keys, float time) {
        if (keys.empty()) return glm::vec3(1.0f);
        if (keys.size() == 1 || time <= keys[0].Time) return keys[0].Value;
        if (time >= keys.back().Time) return keys.back().Value;

        size_t index = FindKeyIndex(keys, time);
        float t = (time - keys[index].Time) / (keys[index + 1].Time - keys[index].Time);
        return glm::mix(keys[index].Value, keys[index + 1].Value, t);
    }

    void OnTick() override {
        float deltaTime = 0.016f; // Default 60fps

        // Get delta time
        for (const auto &[_, dt] : _deltaTimeView.each()) {
            deltaTime = dt.Delta;
        }

        auto scene = _scene.lock();
        if (!scene) return;

        for (const auto &[_, anim, skinnedMesh, transform] : View.each()) {
            if (!anim.Enabled || anim.AnimationGuid.empty()) continue;

            // Populate bone names, offsets, and hierarchy on first access
            if (skinnedMesh.BoneNames.empty()) {
                // Ensure mesh is loaded first (triggers lazy load if needed)
                scene->GetMesh(skinnedMesh.Guid);

                // Now get bone data
                skinnedMesh.BoneNames = scene->GetMeshBoneNames(skinnedMesh.Guid);
                skinnedMesh.BoneOffsets = scene->GetMeshBoneOffsets(skinnedMesh.Guid);
                skinnedMesh.BoneParents = scene->GetMeshBoneParents(skinnedMesh.Guid);
                if (!skinnedMesh.BoneNames.empty()) {
                    std::cout << "Loaded " << skinnedMesh.BoneNames.size() << " bones with hierarchy for mesh" << std::endl;
                }
            }

            // Load animation if not already loaded
            if (_loadedAnimations.find(anim.AnimationGuid) == _loadedAnimations.end()) {
                auto animData = scene->GetAnimation(anim.AnimationGuid);
                if (animData) {
                    _loadedAnimations[anim.AnimationGuid] = animData;
                    std::cout << "Loaded animation: " << animData->Name << " (" << animData->Duration << "s)" << std::endl;
                } else {
                    std::cerr << "Failed to load animation: " << anim.AnimationGuid << std::endl;
                    anim.Enabled = false;
                    continue;
                }
            }

            auto animData = _loadedAnimations[anim.AnimationGuid];
            if (!animData) continue;

            // Update animation time
            anim.Time += deltaTime;
            if (anim.Loop && anim.Time > animData->Duration) {
                anim.Time = fmod(anim.Time, animData->Duration);
            }

            // Create bone name to index map
            std::unordered_map<std::string, int> boneNameToIndex;
            for (size_t i = 0; i < skinnedMesh.BoneNames.size(); i++) {
                boneNameToIndex[skinnedMesh.BoneNames[i]] = static_cast<int>(i);
            }

            // Initialize local transforms storage if needed
            if (skinnedMesh.LocalBoneTransforms.size() != skinnedMesh.BoneNames.size()) {
                skinnedMesh.LocalBoneTransforms.resize(skinnedMesh.BoneNames.size(), glm::mat4(1.0f));
            }

            // Step 1: Collect local transforms from animation
            std::vector<glm::mat4>& localTransforms = skinnedMesh.LocalBoneTransforms;

            for (const auto& channel : animData->Channels) {
                std::string meshBoneName = TryMatchBoneName(channel.BoneName, boneNameToIndex);
                if (meshBoneName.empty()) {
                    continue;
                }

                const int boneIndex = boneNameToIndex[meshBoneName];

                // Sample transformations at current time
                glm::vec3 position = SamplePosition(channel.PositionKeys, anim.Time);
                glm::quat rotation = SampleRotation(channel.RotationKeys, anim.Time);
                glm::vec3 scale = SampleScale(channel.ScaleKeys, anim.Time);

                // Build local transformation matrix
                glm::mat4 localTransform = glm::mat4(1.0f);
                localTransform = glm::translate(localTransform, position);
                localTransform = localTransform * glm::toMat4(rotation);
                localTransform = glm::scale(localTransform, scale);

                localTransforms[boneIndex] = localTransform;
            }

            // Step 2: Compute world-space transforms using bone hierarchy
            // Must process in hierarchical order (parents before children)
            std::vector<glm::mat4> worldTransforms(skinnedMesh.BoneNames.size(), glm::mat4(1.0f));
            std::vector<bool> processed(skinnedMesh.BoneNames.size(), false);

            // Capture bone parents before lambda to avoid C++20 structured binding capture warning
            const auto& boneParents = skinnedMesh.BoneParents;

            std::function<void(int)> computeWorldTransform = [&](int boneIndex) {
                if (processed[boneIndex]) return;

                int parentIndex = boneParents[boneIndex];
                if (parentIndex == -1) {
                    // Root bone - world transform equals local transform
                    worldTransforms[boneIndex] = localTransforms[boneIndex];
                } else {
                    // Ensure parent is processed first
                    computeWorldTransform(parentIndex);
                    // Child bone - world transform = parent's world transform * local transform
                    worldTransforms[boneIndex] = worldTransforms[parentIndex] * localTransforms[boneIndex];
                }
                processed[boneIndex] = true;
            };

            // Process all bones
            for (size_t i = 0; i < skinnedMesh.BoneNames.size(); i++) {
                computeWorldTransform(static_cast<int>(i));
            }

            // Step 3: Apply offset matrices to get final bone transforms
            for (size_t i = 0; i < skinnedMesh.BoneTransforms.size(); i++) {
                if (i < worldTransforms.size() && i < skinnedMesh.BoneOffsets.size()) {
                    // Final transform = world transform * offset (inverse bind pose)
                    skinnedMesh.BoneTransforms[i] = worldTransforms[i] * skinnedMesh.BoneOffsets[i];
                } else {
                    skinnedMesh.BoneTransforms[i] = glm::mat4(1.0f);
                }
            }
        }
    }
};
