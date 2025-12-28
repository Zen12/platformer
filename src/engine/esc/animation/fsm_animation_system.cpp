#define GLM_ENABLE_EXPERIMENTAL
#include "fsm_animation_system.hpp"
#include "../../fsm/fsm_controller.hpp"
#include "../../fsm/fsm_asset.hpp"
#include "../../fsm/fsm_asset_yaml.hpp"

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
        if (!anim.FsmGuid.empty() && _fsmControllers.find(entity) == _fsmControllers.end()) {
            if (auto assetManager = scene->GetAssetManager().lock()) {
                try {
                    auto fsmAsset = assetManager->LoadAssetByGuid<FsmAsset>(anim.FsmGuid);

                    // Create shared_ptr to animation component with custom no-op deleter
                    // (component is owned by ECS registry, not by this shared_ptr)
                    auto animCompPtr = std::shared_ptr<FsmAnimationComponent>(&anim, [](FsmAnimationComponent*){});

                    // Store the shared_ptr to keep it alive
                    _animationComponentPtrs[entity] = animCompPtr;

                    // Create FsmController (managers are nullptr since animation FSMs don't need UI/scene management)
                    _fsmControllers[entity] = std::make_shared<FsmController>(
                        fsmAsset,
                        nullptr,
                        nullptr,
                        animCompPtr
                    );

                    std::cout << "Created FSM controller for entity with FSM: " << fsmAsset.Name << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Failed to load FSM: " << anim.FsmGuid << " - " << e.what() << std::endl;
                }
            }
        }

        // Update FsmController
        if (_fsmControllers.find(entity) != _fsmControllers.end()) {
            _fsmControllers[entity]->Update();
        }

        // Update transition
        if (anim.IsTransitioning) {
            anim.TransitionProgress += deltaTime;
            if (anim.TransitionProgress >= anim.TransitionDuration) {
                // Transition complete
                // Set trigger if specified
                if (!anim.OnCompleteTrigger.empty() && _fsmControllers.find(entity) != _fsmControllers.end()) {
                    _fsmControllers[entity]->SetTrigger(anim.OnCompleteTrigger);
                }

                anim.IsTransitioning = false;
                anim.CurrentAnimationGuid = anim.ToAnimationGuid;
                // Don't reset Time - let animation continue from accumulated time during transition
                anim.FirstFrame = true;
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
                // Set trigger if specified
                if (!anim.OnCompleteTrigger.empty() && _fsmControllers.find(entity) != _fsmControllers.end()) {
                    _fsmControllers[entity]->SetTrigger(anim.OnCompleteTrigger);
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

        // Root motion (only for non-transitioning animations)
        if (!anim.IsTransitioning && anim.ApplyRootMotion && !anim.RootBoneName.empty()) {
            auto rootBoneIt = boneNameToIndex.find(anim.RootBoneName);
            if (rootBoneIt != boneNameToIndex.end()) {
                const int rootBoneIndex = rootBoneIt->second;

                auto animData = _loadedAnimations[anim.CurrentAnimationGuid];
                if (animData) {
                    for (const auto& channel : animData->Channels) {
                        std::string meshBoneName = TryMatchBoneName(channel.BoneName, boneNameToIndex);
                        if (meshBoneName == anim.RootBoneName) {
                            glm::vec3 currentRootPos = SamplePosition(channel.PositionKeys, anim.Time);
                            glm::quat currentRootRot = SampleRotation(channel.RotationKeys, anim.Time);

                            if (anim.FirstFrame) {
                                anim.PreviousRootPosition = currentRootPos;
                                anim.PreviousRootRotation = currentRootRot;
                                anim.FirstFrame = false;
                            } else {
                                glm::vec3 deltaPos = currentRootPos - anim.PreviousRootPosition;
                                deltaPos.y = 0.0f;
                                transform.AddPosition(deltaPos);
                                anim.PreviousRootPosition = currentRootPos;
                                anim.PreviousRootRotation = currentRootRot;

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
