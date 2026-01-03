#define GLM_ENABLE_EXPERIMENTAL
#include "fsm_animation_system.hpp"
#include "../../fsm/fsm_controller.hpp"
#include "../../fsm/fsm_asset.hpp"
#include "../../fsm/fsm_asset_yaml.hpp"

#define DEBUG_FSM_ANIM_SYSTEM 0

#if DEBUG_FSM_ANIM_SYSTEM
#define FSM_ANIM_LOG if(1) std::cout
#else
#define FSM_ANIM_LOG if(0) std::cout
#endif

FsmAnimationSystem::~FsmAnimationSystem() = default;

void FsmAnimationSystem::OnTick() {
    float deltaTime = 0.016f;

    for (const auto &[_, dt] : _deltaTimeView.each()) {
        deltaTime = dt.Delta;
    }

    auto scene = _scene.lock();
    if (!scene) return;

    for (const auto &[entity, anim, skinnedMesh, transform] : View.each()) {
        // Create FsmController if not exists
        if (!anim.FsmGuid.empty() && !anim.Controller) {
            if (auto assetManager = scene->GetAssetManager().lock()) {
                try {
                    auto fsmAsset = assetManager->LoadAssetByGuid<FsmAsset>(anim.FsmGuid);

                    // Create shared_ptr to animation component with custom no-op deleter
                    // (component is owned by ECS registry, not by this shared_ptr)
                    anim.SelfPtr = std::shared_ptr<FsmAnimationComponent>(&anim, [](FsmAnimationComponent*){});

                    // Create FsmController (managers are nullptr since animation FSMs don't need UI/scene management)
                    anim.Controller = std::make_shared<FsmController>(
                        fsmAsset,
                        nullptr,
                        nullptr,
                        nullptr,  // videoRecorder (animation FSMs don't need it)
                        anim.SelfPtr
                    );

                } catch (const std::exception& e) {
                    std::cerr << "Failed to load FSM: " << anim.FsmGuid << " - " << e.what() << std::endl;
                }
            }
        }

        // Update FsmController
        if (anim.Controller) {
            anim.Controller->Update();
        }

        // Update transition
        if (anim.IsTransitioning) {
            anim.TransitionProgress += deltaTime;
            if (anim.TransitionProgress >= anim.TransitionDuration) {
                // Transition complete
                // Set trigger if specified
                if (!anim.OnCompleteTrigger.empty() && anim.Controller) {
                    anim.Controller->SetTrigger(anim.OnCompleteTrigger);
                }

                FSM_ANIM_LOG << "[ANIM] Transition complete: " << anim.FromAnimationGuid
                             << " -> " << anim.ToAnimationGuid << std::endl;
                anim.IsTransitioning = false;
                anim.CurrentAnimationGuid = anim.ToAnimationGuid;
                // Don't reset Time - let animation continue from accumulated time during transition
                anim.TransitionProgress = 0.0f;
                anim.HasCompletedOnce = false;  // Reset for next animation state
            }
        }

        // Skip bone calculation if no animation is set
        if (anim.CurrentAnimationGuid.empty()) continue;

        // Populate bone data
        if (skinnedMesh.BoneNames.empty()) {
            scene->GetMesh(skinnedMesh.Guid);
            skinnedMesh.BoneNames = scene->GetMeshBoneNames(skinnedMesh.Guid);
            skinnedMesh.BoneOffsets = scene->GetMeshBoneOffsets(skinnedMesh.Guid);
            skinnedMesh.BoneParents = scene->GetMeshBoneParents(skinnedMesh.Guid);
        }

        // Bone calculations
        std::unordered_map<std::string, int> boneNameToIndex;
        for (size_t i = 0; i < skinnedMesh.BoneNames.size(); i++) {
            boneNameToIndex[skinnedMesh.BoneNames[i]] = static_cast<int>(i);
        }

        if (skinnedMesh.LocalBoneTransforms.size() != skinnedMesh.BoneNames.size()) {
            skinnedMesh.LocalBoneTransforms.resize(skinnedMesh.BoneNames.size(), glm::mat4(1.0f));
        }

        std::vector<glm::mat4>& localTransforms = skinnedMesh.LocalBoneTransforms;

        if (anim.IsTransitioning) {
            // Load both animations for blending
            if (_loadedAnimations.find(anim.FromAnimationGuid) == _loadedAnimations.end()) {
                auto fromAnimData = scene->GetAnimation(anim.FromAnimationGuid);
                if (fromAnimData) _loadedAnimations[anim.FromAnimationGuid] = fromAnimData;
            }
            if (_loadedAnimations.find(anim.ToAnimationGuid) == _loadedAnimations.end()) {
                auto toAnimData = scene->GetAnimation(anim.ToAnimationGuid);
                if (toAnimData) _loadedAnimations[anim.ToAnimationGuid] = toAnimData;
            }

            auto fromAnimData = _loadedAnimations[anim.FromAnimationGuid];
            auto toAnimData = _loadedAnimations[anim.ToAnimationGuid];
            if (!fromAnimData || !toAnimData) continue;

            // Update animation time
            anim.Time += deltaTime;
            if (anim.Loop && anim.Time > toAnimData->Duration) {
                anim.Time = fmod(anim.Time, toAnimData->Duration);
            }

            // Calculate blend factor (0.0 = from animation, 1.0 = to animation)
            float blendFactor = anim.TransitionProgress / anim.TransitionDuration;
            blendFactor = glm::clamp(blendFactor, 0.0f, 1.0f);

            // Blend bone transforms
            std::unordered_map<std::string, glm::vec3> fromPositions;
            std::unordered_map<std::string, glm::quat> fromRotations;
            std::unordered_map<std::string, glm::vec3> fromScales;

            // Sample from animation
            for (const auto& channel : fromAnimData->Channels) {
                fromPositions[channel.BoneName] = SamplePosition(channel.PositionKeys, anim.Time);
                fromRotations[channel.BoneName] = SampleRotation(channel.RotationKeys, anim.Time);
                fromScales[channel.BoneName] = SampleScale(channel.ScaleKeys, anim.Time);
            }

            // Sample to animation and blend
            for (const auto& channel : toAnimData->Channels) {
                std::string meshBoneName = TryMatchBoneName(channel.BoneName, boneNameToIndex);
                if (meshBoneName.empty()) continue;

                const int boneIndex = boneNameToIndex[meshBoneName];

                glm::vec3 toPosition = SamplePosition(channel.PositionKeys, anim.Time);
                glm::quat toRotation = SampleRotation(channel.RotationKeys, anim.Time);
                glm::vec3 toScale = SampleScale(channel.ScaleKeys, anim.Time);

                // Blend with from animation if available
                glm::vec3 position = toPosition;
                glm::quat rotation = toRotation;
                glm::vec3 scale = toScale;

                if (fromPositions.find(channel.BoneName) != fromPositions.end()) {
                    position = glm::mix(fromPositions[channel.BoneName], toPosition, blendFactor);
                    rotation = glm::slerp(fromRotations[channel.BoneName], toRotation, blendFactor);
                    scale = glm::mix(fromScales[channel.BoneName], toScale, blendFactor);
                }

                glm::mat4 localTransform = glm::mat4(1.0f);
                localTransform = glm::translate(localTransform, position);
                localTransform = localTransform * glm::toMat4(rotation);
                localTransform = glm::scale(localTransform, scale);

                localTransforms[boneIndex] = localTransform;
            }
        } else {
            // Normal animation (no transition)
            if (_loadedAnimations.find(anim.CurrentAnimationGuid) == _loadedAnimations.end()) {
                auto animData = scene->GetAnimation(anim.CurrentAnimationGuid);
                if (animData) {
                    _loadedAnimations[anim.CurrentAnimationGuid] = animData;
                    FSM_ANIM_LOG << "[ANIM] Loaded animation: " << anim.CurrentAnimationGuid
                                 << " Duration: " << animData->Duration << "s" << std::endl;
                } else {
                    std::cerr << "Failed to load animation: " << anim.CurrentAnimationGuid << std::endl;
                    continue;
                }
            }

            auto animData = _loadedAnimations[anim.CurrentAnimationGuid];
            if (!animData) continue;

            // Update animation time
            anim.Time += deltaTime;

            // Check for animation completion
            if (anim.Time >= animData->Duration && !anim.HasCompletedOnce) {
                anim.HasCompletedOnce = true;
                FSM_ANIM_LOG << "[ANIM] Animation completed: " << anim.CurrentAnimationGuid
                             << " Trigger: " << anim.OnCompleteTrigger << std::endl;
                // Set trigger if specified
                if (!anim.OnCompleteTrigger.empty() && anim.Controller) {
                    anim.Controller->SetTrigger(anim.OnCompleteTrigger);
                }
            }

            if (anim.Loop && anim.Time > animData->Duration) {
                anim.Time = fmod(anim.Time, animData->Duration);
                anim.HasCompletedOnce = false;  // Reset for next loop
            }

            for (const auto& channel : animData->Channels) {
                std::string meshBoneName = TryMatchBoneName(channel.BoneName, boneNameToIndex);
                if (meshBoneName.empty()) continue;

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
        }

        // Compute world transforms
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

        // Apply offsets
        for (size_t i = 0; i < skinnedMesh.BoneTransforms.size(); i++) {
            if (i < worldTransforms.size() && i < skinnedMesh.BoneOffsets.size()) {
                skinnedMesh.BoneTransforms[i] = worldTransforms[i] * skinnedMesh.BoneOffsets[i];
            } else {
                skinnedMesh.BoneTransforms[i] = glm::mat4(1.0f);
            }
        }
    }
}
