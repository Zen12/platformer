#pragma once
#include "../esc_core.hpp"
#include "ozz_animation_component.hpp"
#include "ozz_skeleton_component.hpp"
#include "ozz_ik_component.hpp"
#include "../skinned_mesh_renderer/skinned_mesh_renderer_component.hpp"
#include "../time/time_component.hpp"
#include "../../scene/scene.hpp"
#include <ozz/animation/runtime/animation.h>
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/animation/runtime/local_to_model_job.h>
#include <ozz/animation/runtime/ik_two_bone_job.h>
#include <ozz/animation/runtime/ik_aim_job.h>
#include <ozz/base/maths/soa_transform.h>
#include <ozz/base/maths/simd_math.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <memory>
#include <iostream>

// Ozz Animation System
// Handles animation sampling using ozz-animation library
// Supports skeletal animation with proper hierarchy handling
class OzzAnimationSystem final : public ISystemView<OzzAnimationComponent, OzzSkeletonComponent, SkinnedMeshRendererComponent> {
private:
    using TypeDeltaTime = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());
    const TypeDeltaTime _deltaTimeView;
    const std::weak_ptr<Scene> _scene;

    // Cached ozz animations (shared across instances)
    std::unordered_map<std::string, std::shared_ptr<ozz::animation::Animation>> _loadedAnimations;

    // Helper: Convert ozz::math::Float4x4 to glm::mat4
    static glm::mat4 OzzToGlm(const ozz::math::Float4x4& ozz_mat) {
        // Ozz stores matrices in column-major format, same as GLM
        // Use ozz's portable GetFloat methods to extract values
        glm::mat4 glm_mat;
        const float* data = reinterpret_cast<const float*>(&ozz_mat);
        for (int i = 0; i < 16; ++i) {
            glm_mat[i / 4][i % 4] = data[i];
        }
        return glm_mat;
    }

    // Helper: Convert glm::mat4 to ozz::math::Float4x4
    static ozz::math::Float4x4 GlmToOzz(const glm::mat4& glm_mat) {
        ozz::math::Float4x4 ozz_mat;
        float* data = reinterpret_cast<float*>(&ozz_mat);
        for (int i = 0; i < 16; ++i) {
            data[i] = glm_mat[i / 4][i % 4];
        }
        return ozz_mat;
    }

    // Helper: Convert glm::vec3 to ozz::math::SimdFloat4 (for IK targets/vectors)
    static ozz::math::SimdFloat4 GlmToOzzSimdFloat4(const glm::vec3& v) {
        return ozz::math::simd_float4::Load(v.x, v.y, v.z, 0.0f);
    }

    // Helper: Map Mixamo bone names to simplified mesh bone names
    static std::string MapMixamoBoneName(const std::string& mixamoName) {
        static const std::unordered_map<std::string, std::string> mixamoToMesh = {
            {"mixamorig:Hips", "Hips"},
            {"mixamorig:Spine", "Spine"},
            {"mixamorig:Spine1", "Chest"},  // Mixamo has Spine, Spine1, Spine2 - map Spine1 to Chest
            {"mixamorig:Spine2", "Chest"},  // Some animations might use Spine2
            {"mixamorig:Neck", "Neck"},
            {"mixamorig:Head", "Head"},
            {"mixamorig:LeftArm", "UpperArm.L"},
            {"mixamorig:LeftForeArm", "Forearm.L"},
            {"mixamorig:LeftHand", "Hand.L"},
            {"mixamorig:RightArm", "UpperArm.R"},
            {"mixamorig:RightForeArm", "Forearm.R"},
            {"mixamorig:RightHand", "Hand.R"},
            {"mixamorig:LeftUpLeg", "Thigh.L"},
            {"mixamorig:LeftLeg", "Shin.L"},
            {"mixamorig:LeftFoot", "Foot.L"},
            {"mixamorig:RightUpLeg", "Thigh.R"},
            {"mixamorig:RightLeg", "Shin.R"},
            {"mixamorig:RightFoot", "Foot.R"}
        };

        auto it = mixamoToMesh.find(mixamoName);
        if (it != mixamoToMesh.end()) {
            return it->second;
        }
        return mixamoName;  // Return original if no mapping found
    }

    // Helper: Find bone index by name
    static int FindBoneIndex(const ozz::animation::Skeleton& skeleton,
                            const std::vector<std::string>& boneNames,
                            const std::string& boneName) {
        // Try exact match first
        for (size_t i = 0; i < boneNames.size(); ++i) {
            if (boneNames[i] == boneName) {
                return static_cast<int>(i);
            }
        }

        // Try without "mixamorig:" prefix
        std::string strippedName = boneName;
        if (strippedName.find("mixamorig:") == 0) {
            strippedName = strippedName.substr(10);
            for (size_t i = 0; i < boneNames.size(); ++i) {
                if (boneNames[i] == strippedName) {
                    return static_cast<int>(i);
                }
            }
        }

        return -1;
    }

public:
    explicit OzzAnimationSystem(const TypeView& view, const TypeDeltaTime& deltaTimeView, const std::weak_ptr<Scene>& scene)
        : ISystemView(view), _deltaTimeView(deltaTimeView), _scene(scene) {
    }

    void OnTick() override {
        float deltaTime = 0.016f; // Default 60fps

        // Get delta time
        for (const auto& [_, dt] : _deltaTimeView.each()) {
            deltaTime = dt.Delta;
        }

        auto scene = _scene.lock();
        if (!scene) return;

        // Get registry from scene
        entt::registry* registry = nullptr;
        if (scene) {
            if (auto reg = scene->GetEntityRegistry()) {
                registry = reg.get();
            }
        }

        for (const auto& [entity, animComp, skelComp, skinnedMesh] : View.each()) {
            if (!animComp.Playing || animComp.AnimationGuid.empty()) continue;

            // Get optional IK component
            OzzIKComponent* ikComp = nullptr;
            if (registry) {
                ikComp = registry->try_get<OzzIKComponent>(entity);
            }

            // Load skeleton if not loaded
            if (!skelComp.Skeleton && !skelComp.SkeletonGuid.empty()) {
                skelComp.Skeleton = scene->GetOzzSkeleton(skelComp.SkeletonGuid);
                if (skelComp.Skeleton) {
                    const_cast<OzzSkeletonComponent&>(skelComp).AllocateBuffers(*skelComp.Skeleton);
                }
            }

            // Load bone offsets lazily when mesh is loaded (CRITICAL: inverse bind pose matrices)
            // This must be after skeleton loading because mesh might not be loaded yet
            if (skinnedMesh.BoneOffsets.empty()) {
                auto boneOffsets = scene->GetMeshBoneOffsets(skinnedMesh.Guid);
                auto meshBoneNames = scene->GetMeshBoneNames(skinnedMesh.Guid);
                if (!boneOffsets.empty()) {
                    // CRITICAL: Must get mutable reference from registry to actually modify the component!
                    // const_cast doesn't persist across frames - ECS gives us a new const ref each frame
                    auto& mutableMesh = registry->get<SkinnedMeshRendererComponent>(entity);
                    mutableMesh.BoneOffsets = boneOffsets;
                    mutableMesh.BoneNames = meshBoneNames;
                }
            }

            if (!skelComp.Skeleton) {
                continue;
            }

            const ozz::animation::Skeleton& skeleton = *skelComp.Skeleton;

            // Load animation if not cached
            if (_loadedAnimations.find(animComp.AnimationGuid) == _loadedAnimations.end()) {
                auto animation = scene->GetOzzAnimation(animComp.AnimationGuid);
                if (animation) {
                    _loadedAnimations[animComp.AnimationGuid] = animation;
                } else {
                    animComp.Playing = false;
                    continue;
                }
            }

            auto animation = _loadedAnimations[animComp.AnimationGuid];
            if (!animation) continue;

            // Update animation time
            animComp.Time += deltaTime * animComp.PlaybackSpeed;
            if (animComp.Loop && animComp.Time > animation->duration()) {
                animComp.Time = fmod(animComp.Time, animation->duration());
            } else if (animComp.Time > animation->duration()) {
                animComp.Time = animation->duration();
                animComp.Playing = false;  // Stop when animation ends
            }


            // Ensure transform buffers are allocated
            if (skelComp.LocalTransforms.empty()) {
                const_cast<OzzSkeletonComponent&>(skelComp).AllocateBuffers(skeleton);
            }

            // Sample animation using ozz sampling job
            ozz::animation::SamplingJob sampling_job;
            sampling_job.animation = animation.get();
            sampling_job.context = skelComp.SamplingCache.get();  // Use cache for proper operation
            sampling_job.ratio = animComp.Time / animation->duration();
            sampling_job.output = ozz::span(const_cast<ozz::math::SoaTransform*>(skelComp.LocalTransforms.data()),
                                            skelComp.LocalTransforms.size());

            if (!sampling_job.Run()) {
                std::cerr << "OzzAnimationSystem: Sampling job failed" << std::endl;
                continue;
            }

            // Apply mouse IK to local transforms BEFORE converting to model space
            // This ensures child bones update when parent rotates
            if (ikComp && ikComp->Enabled) {
                // Resolve bone index from name if not already resolved
                if (ikComp->StartBoneIndex < 0 && !ikComp->StartBoneName.empty()) {
                    ikComp->StartBoneIndex = FindBoneIndex(skeleton, skelComp.BoneNames, ikComp->StartBoneName);
                }

                if (ikComp->StartBoneIndex >= 0 && ikComp->StartBoneIndex < (int)skelComp.LocalTransforms.size()) {
                    // Get mouse-based rotation angles directly from target
                    // Target.x and Target.y are driven by mouse screen space
                    float rotX = ikComp->Target.x * 0.002f;  // Reduced multiplier for smoother control
                    float rotY = ikComp->Target.y * 0.002f;

                    // Access the SoA (Structure of Arrays) local transform for this bone
                    // ozz uses SoA format where 4 bones are stored together for SIMD efficiency
                    const int soaIndex = ikComp->StartBoneIndex / 4;  // Which SoA block
                    const int soaOffset = ikComp->StartBoneIndex % 4; // Which lane in the block

                    if (soaIndex < (int)skelComp.LocalTransforms.size()) {
                        auto& localTransform = const_cast<ozz::math::SoaTransform&>(skelComp.LocalTransforms[soaIndex]);

                        // Extract current rotation quaternion for this bone
                        ozz::math::SimdFloat4 qx = localTransform.rotation.x;
                        ozz::math::SimdFloat4 qy = localTransform.rotation.y;
                        ozz::math::SimdFloat4 qz = localTransform.rotation.z;
                        ozz::math::SimdFloat4 qw = localTransform.rotation.w;

                        // Extract scalar values - StorePtr writes 4 floats (16 bytes)
                        float qxValues[4], qyValues[4], qzValues[4], qwValues[4];
                        ozz::math::StorePtr(qx, qxValues);
                        ozz::math::StorePtr(qy, qyValues);
                        ozz::math::StorePtr(qz, qzValues);
                        ozz::math::StorePtr(qw, qwValues);

                        // Get the specific bone's quaternion components (based on SoA offset)
                        float currentQx = qxValues[soaOffset];
                        float currentQy = qyValues[soaOffset];
                        float currentQz = qzValues[soaOffset];
                        float currentQw = qwValues[soaOffset];

                        glm::quat currentRot(currentQw, currentQx, currentQy, currentQz);

                        // Create rotation delta from mouse input
                        glm::quat deltaRotX = glm::angleAxis(rotX, glm::vec3(1, 0, 0));
                        glm::quat deltaRotY = glm::angleAxis(rotY, glm::vec3(0, 1, 0));
                        glm::quat deltaRot = deltaRotY * deltaRotX;

                        // Apply delta rotation
                        glm::quat newRot = deltaRot * currentRot;
                        newRot = glm::normalize(newRot);

                        // Update the SoA with new rotation - reuse the arrays we already extracted
                        qxValues[soaOffset] = newRot.x;
                        qyValues[soaOffset] = newRot.y;
                        qzValues[soaOffset] = newRot.z;
                        qwValues[soaOffset] = newRot.w;

                        // Rebuild SoA quaternion from updated values
                        localTransform.rotation.x = ozz::math::simd_float4::Load(qxValues[0], qxValues[1], qxValues[2], qxValues[3]);
                        localTransform.rotation.y = ozz::math::simd_float4::Load(qyValues[0], qyValues[1], qyValues[2], qyValues[3]);
                        localTransform.rotation.z = ozz::math::simd_float4::Load(qzValues[0], qzValues[1], qzValues[2], qzValues[3]);
                        localTransform.rotation.w = ozz::math::simd_float4::Load(qwValues[0], qwValues[1], qwValues[2], qwValues[3]);

                    }
                }
            }

            // Convert local transforms to model space (world space relative to root)
            // This happens AFTER IK so child bones properly inherit parent rotations
            ozz::animation::LocalToModelJob ltm_job;
            ltm_job.skeleton = &skeleton;
            ltm_job.input = ozz::span(skelComp.LocalTransforms.data(), skelComp.LocalTransforms.size());
            ltm_job.output = ozz::span(const_cast<ozz::math::Float4x4*>(skelComp.ModelMatrices.data()),
                                      skelComp.ModelMatrices.size());

            if (!ltm_job.Run()) {
                std::cerr << "OzzAnimationSystem: LocalToModel job failed" << std::endl;
                continue;
            }


            // Convert ozz matrices to glm (WITHOUT applying bone offsets yet!)
            // Bone offsets must be applied AFTER remapping to mesh indices
            for (size_t i = 0; i < skelComp.ModelMatrices.size() && i < skelComp.BoneMatrices.size(); ++i) {
                glm::mat4 boneMatrix = OzzToGlm(skelComp.ModelMatrices[i]);
                skelComp.BoneMatrices[i] = boneMatrix;
            }

            // Copy bone matrices to skinned mesh renderer with BONE REMAPPING
            // CRITICAL: Skeleton bone indices != Mesh bone indices! Must remap by name!

            // Get mutable mesh reference from registry (not const ref from view!)
            // This is CRITICAL - const_cast doesn't persist across frames
            auto& mutableMesh = registry->get<SkinnedMeshRendererComponent>(entity);

            // Skip remapping if bone offsets aren't loaded yet - transforms would be wrong!
            if (mutableMesh.BoneOffsets.empty() || mutableMesh.BoneNames.empty()) {
                continue; // Wait for lazy load to complete
            }

            // Build skeleton name -> index map with Mixamo name mapping
            std::unordered_map<std::string, int> skelNameToIndex;
            for (size_t i = 0; i < skelComp.BoneNames.size(); ++i) {
                if (!skelComp.BoneNames[i].empty()) {
                    // Store both original name AND mapped name
                    skelNameToIndex[skelComp.BoneNames[i]] = static_cast<int>(i);

                    // Also store Mixamo-mapped name if different
                    std::string mappedName = MapMixamoBoneName(skelComp.BoneNames[i]);
                    if (mappedName != skelComp.BoneNames[i]) {
                        skelNameToIndex[mappedName] = static_cast<int>(i);
                    }
                }
            }


            // Find Root bone transform to use as fallback for unmapped bones
            glm::mat4 rootTransform = glm::mat4(1.0f);
            auto rootIt = skelNameToIndex.find("Root");
            if (rootIt != skelNameToIndex.end() && rootIt->second < (int)skelComp.BoneMatrices.size()) {
                rootTransform = skelComp.BoneMatrices[rootIt->second];
            }

            // Remap: For each mesh bone, find matching skeleton bone and copy transform
            auto& boneTransforms = mutableMesh.BoneTransforms;

            for (size_t meshIdx = 0; meshIdx < mutableMesh.BoneNames.size() && meshIdx < boneTransforms.size(); ++meshIdx) {
                const std::string& meshBoneName = mutableMesh.BoneNames[meshIdx];
                auto it = skelNameToIndex.find(meshBoneName);
                if (it != skelNameToIndex.end()) {
                    int skelIdx = it->second;
                    if (skelIdx < (int)skelComp.BoneMatrices.size()) {
                        // Copy skeleton bone transform to correct mesh bone index
                        glm::mat4 boneMatrix = skelComp.BoneMatrices[skelIdx];

                        // NOW apply inverse bind pose in mesh space (correct indices!)
                        if (meshIdx < mutableMesh.BoneOffsets.size()) {
                            boneMatrix = boneMatrix * mutableMesh.BoneOffsets[meshIdx];
                        }

                        boneTransforms[meshIdx] = boneMatrix;
                    }
                } else {
                    // Unmapped bone (limbs not in skeleton) - use Root transform so they follow the body
                    boneTransforms[meshIdx] = rootTransform;
                }
            }
        }
    }
};
