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
    // Handles different naming conventions (Mixamo -> Rigify, etc.)
    static std::string TryMatchBoneName(const std::string& animBoneName, const std::unordered_map<std::string, int>& boneMap) {
        // Try exact match first
        if (boneMap.find(animBoneName) != boneMap.end()) {
            return animBoneName;
        }

        // Strip "mixamorig:" prefix
        std::string boneName = animBoneName;
        const std::string mixamoPrefix = "mixamorig:";
        if (boneName.substr(0, mixamoPrefix.length()) == mixamoPrefix) {
            boneName = boneName.substr(mixamoPrefix.length());
        }

        // Try direct match after prefix strip
        if (boneMap.find(boneName) != boneMap.end()) {
            return boneName;
        }

        // Mixamo -> Rigify bone name mapping
        static const std::unordered_map<std::string, std::string> mixamoToRigify = {
            // Central bones
            {"Hips", "hip"},
            {"Spine", "hip"},  // Map to hip as base
            {"Spine1", "chest"},
            {"Spine2", "chest"},  // Both spine bones map to chest
            {"Neck", "neck"},
            {"Head", "head"},

            // Left arm
            {"LeftShoulder", "KTF.L"},
            {"LeftArm", "upperarm.L"},
            {"LeftForeArm", "lowerarm.L"},
            {"LeftHand", "hand.L"},

            // Right arm
            {"RightShoulder", "KTF.R"},
            {"RightArm", "upperarm.R"},
            {"RightForeArm", "lowerarm.R"},
            {"RightHand", "hand.R"},

            // Left leg
            {"LeftUpLeg", "upperleg.L"},
            {"LeftLeg", "lowerleg.L"},
            {"LeftFoot", "foot.L"},
            {"LeftToeBase", "toose.L"},

            // Right leg
            {"RightUpLeg", "upperleg.R"},
            {"RightLeg", "lowerleg.R"},
            {"RightFoot", "foot.R"},
            {"RightToeBase", "toose.R"},

            // Fingers - Left hand
            {"LeftHandThumb1", "thumb.01.L"},
            {"LeftHandThumb2", "thumb.02.L"},
            {"LeftHandIndex1", "f_index.01.L"},
            {"LeftHandIndex2", "f_index.02.L"},
            {"LeftHandMiddle1", "f_middle.01.L"},
            {"LeftHandMiddle2", "f_middle.02.L"},
            {"LeftHandRing1", "f_ring.01.L"},
            {"LeftHandRing2", "f_ring.02.L"},
            {"LeftHandPinky1", "f_pinky.01.L"},
            {"LeftHandPinky2", "f_pinky.02.L"},

            // Fingers - Right hand
            {"RightHandThumb1", "thumb.01.R"},
            {"RightHandThumb2", "thumb.02.R"},
            {"RightHandIndex1", "f_index.01.R"},
            {"RightHandIndex2", "f_index.02.R"},
            {"RightHandMiddle1", "f_middle.01.R"},
            {"RightHandMiddle2", "f_middle.02.R"},
            {"RightHandRing1", "f_ring.01.R"},
            {"RightHandRing2", "f_ring.02.R"},
            {"RightHandPinky1", "f_pinky.01.R"},
            {"RightHandPinky2", "f_pinky.02.R"},
        };

        // Try Mixamo -> Rigify mapping
        auto it = mixamoToRigify.find(boneName);
        if (it != mixamoToRigify.end()) {
            const std::string& rigifyName = it->second;
            if (boneMap.find(rigifyName) != boneMap.end()) {
                return rigifyName;
            }
        }

        // No match found
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

            // Root motion extraction and application
            if (anim.ApplyRootMotion && !anim.RootBoneName.empty()) {
                auto rootBoneIt = boneNameToIndex.find(anim.RootBoneName);
                if (rootBoneIt != boneNameToIndex.end()) {
                    const int rootBoneIndex = rootBoneIt->second;

                    // Find the root bone channel in animation
                    for (const auto& channel : animData->Channels) {
                        std::string meshBoneName = TryMatchBoneName(channel.BoneName, boneNameToIndex);
                        if (meshBoneName == anim.RootBoneName) {
                            // Sample current root position and rotation
                            glm::vec3 currentRootPos = SamplePosition(channel.PositionKeys, anim.Time);
                            glm::quat currentRootRot = SampleRotation(channel.RotationKeys, anim.Time);

                            if (anim.FirstFrame) {
                                // First frame - just store, don't apply
                                anim.PreviousRootPosition = currentRootPos;
                                anim.PreviousRootRotation = currentRootRot;
                                anim.FirstFrame = false;
                            } else {
                                // Calculate delta movement (XZ only for horizontal movement)
                                glm::vec3 deltaPos = currentRootPos - anim.PreviousRootPosition;
                                deltaPos.y = 0.0f; // Ignore vertical movement

                                // Apply delta directly without rotation transform
                                transform.AddPosition(deltaPos);

                                // Store current as previous
                                anim.PreviousRootPosition = currentRootPos;
                                anim.PreviousRootRotation = currentRootRot;

                                // Remove root motion from bone animation (set to zero delta)
                                // This prevents the character from "sliding" while also moving
                                glm::vec3 position(0.0f);
                                glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                                glm::vec3 scale = SampleScale(channel.ScaleKeys, anim.Time);

                                glm::mat4 localTransform = glm::mat4(1.0f);
                                localTransform = glm::translate(localTransform, position);
                                localTransform = localTransform * glm::toMat4(rotation);
                                localTransform = glm::scale(localTransform, scale);
                                localTransforms[rootBoneIndex] = localTransform;
                            }
                            break;
                        }
                    }
                }
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
