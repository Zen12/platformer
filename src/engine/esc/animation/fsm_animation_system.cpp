#define GLM_ENABLE_EXPERIMENTAL
#include "fsm_animation_system.hpp"
#include "../navmesh_agent/navmesh_agent_component.hpp"
#include "../../fsm/fsm_factory.hpp"
#include "../../fsm/fsm_asset.hpp"
#include "../../fsm/fsm_asset_yaml.hpp"
#include <iomanip>

#define DEBUG_FSM_ANIM_SYSTEM 0
#define DEBUG_VELOCITY_ANIM 0
#define DEBUG_JUMP_TRIGGER 0
#define DEBUG_DIRECTIONAL_ANIM 1

#if DEBUG_DIRECTIONAL_ANIM
#define DIR_ANIM_LOG if(1) std::cout
#else
#define DIR_ANIM_LOG if(0) std::cout
#endif

#if DEBUG_JUMP_TRIGGER
#define JUMP_LOG if(1) std::cout
#else
#define JUMP_LOG if(0) std::cout
#endif

#if DEBUG_FSM_ANIM_SYSTEM
#define FSM_ANIM_LOG if(1) std::cout
#else
#define FSM_ANIM_LOG if(0) std::cout
#endif

#if DEBUG_VELOCITY_ANIM
#define VELOCITY_ANIM_LOG if(1) std::cout
#else
#define VELOCITY_ANIM_LOG if(0) std::cout
#endif

FsmAnimationSystem::~FsmAnimationSystem() = default;

void FsmAnimationSystem::OnTick() {
    float deltaTime = 0.016f;

    for (const auto &[_, dt] : _deltaTimeView.each()) {
        deltaTime = dt.Delta;
    }

    auto scene = _scene.lock();
    if (!scene) return;

    static int velBasedCounter = 0;
    for (const auto &[entity, anim, skinnedMesh, transform] : View.each()) {
        // Calculate effective animation speed
        // Use state override if set, otherwise use component default
        float effectiveSpeed = (anim.StateAnimationSpeed >= 0.0f) ? anim.StateAnimationSpeed : anim.AnimationSpeed;

        // Apply velocity-based speed unless disabled for this state
        bool useVelocitySpeed = anim.VelocityBasedSpeed && !anim.StateDisableVelocitySpeed;
        if (useVelocitySpeed) {
            velBasedCounter++;
            // Check if entity has NavmeshAgentComponent
            if (const auto* navAgent = _registry.try_get<NavmeshAgentComponent>(entity)) {
                // Scale animation speed based on velocity ratio
                const float speedRatio = navAgent->MaxSpeed > 0.0f
                    ? navAgent->CurrentSpeed / navAgent->MaxSpeed
                    : 0.0f;
                effectiveSpeed *= speedRatio * anim.VelocitySpeedScale;

                if (velBasedCounter % 2000 == 1) {
                    VELOCITY_ANIM_LOG << "[ANIM VEL] Speed=" << navAgent->CurrentSpeed
                              << " Ratio=" << speedRatio
                              << " EffectiveSpeed=" << effectiveSpeed << std::endl;
                }
            } else {
                if (velBasedCounter % 2000 == 1) {
                    VELOCITY_ANIM_LOG << "[ANIM VEL] VelBased=TRUE but No NavmeshAgentComponent!" << std::endl;
                }
            }
        }

        // Check if directional blending is active (only when moving AND grounded)
        bool useDirectionalBlend = false;
        float directionAngle = 0.0f;
        bool isMoving = false;
        bool isGrounded = true;

        // Check if entity is moving and grounded
        if (const auto* navAgent = _registry.try_get<NavmeshAgentComponent>(entity)) {
            constexpr float moveThreshold = 0.5f;
            isMoving = navAgent->CurrentSpeed > moveThreshold;
            isGrounded = navAgent->IsGrounded;
        }

        // Only use directional blending when moving AND grounded (not jumping)
        if (isMoving && isGrounded) {
            if (const auto* ikAim = _registry.try_get<IKAimComponent>(entity)) {
                if (ikAim->RotateCharacterToAim && ikAim->Enabled) {
                    directionAngle = ikAim->MovementDirectionAngle;
                    // Use directional blending if configured, otherwise fall back to simple reversal
                    if (anim.UseDirectionalBlending &&
                        !anim.DirectionalWalkForwardGuid.empty() &&
                        !anim.DirectionalWalkBackGuid.empty()) {
                        useDirectionalBlend = true;
                    } else {
                        // Fallback: Reverse animation when moving backward (angle > 90 degrees)
                        float absAngle = std::abs(directionAngle);
                        if (absAngle > 90.0f) {
                            effectiveSpeed = -effectiveSpeed;
                        }
                    }
                }
            }
        }

        // Create FsmController if not exists
        if (!anim.FsmGuid.empty() && !anim.Controller) {
            if (auto assetManager = scene->GetAssetManager().lock()) {
                try {
                    auto fsmAsset = assetManager->LoadAssetByGuid<FsmAsset>(anim.FsmGuid);

                    // Create shared_ptr to animation component with custom no-op deleter
                    // (component is owned by ECS registry, not by this shared_ptr)
                    anim.SelfPtr = std::shared_ptr<FsmAnimationComponent>(&anim, [](FsmAnimationComponent*){});

                    // Get AudioManager from scene for footstep sounds etc.
                    std::shared_ptr<AudioManager> audioManager = nullptr;
                    if (auto am = scene->GetAudioManager().lock()) {
                        audioManager = am;
                    }

                    // Create FsmController (managers are nullptr except audioManager for sounds)
                    anim.Controller = FsmFactory::Create(
                        fsmAsset,
                        nullptr,
                        nullptr,
                        nullptr,  // videoRecorder (animation FSMs don't need it)
                        audioManager,  // audioManager for footstep sounds
                        anim.SelfPtr
                    );

                } catch (const std::exception& e) {
                    std::cerr << "Failed to load FSM: " << anim.FsmGuid << " - " << e.what() << std::endl;
                }
            }
        }

        // Set animation triggers based on navmesh agent state
        if (anim.Controller) {
            if (auto* navAgent = _registry.try_get<NavmeshAgentComponent>(entity)) {
                // Trigger jump animation when player starts jumping (one-shot)
                if (navAgent->IsJumping && !anim.WasJumping) {
                    JUMP_LOG << "[JUMP] Triggering jump! IsJumping=" << navAgent->IsJumping
                             << " WasJumping=" << anim.WasJumping << std::endl;
                    anim.Controller->SetTrigger("jump");
                }
                // Trigger landing when player lands (using JustLanded for one-frame detection)
                if (navAgent->JustLanded) {
                    JUMP_LOG << "[JUMP] Triggering landed!" << std::endl;
                    anim.Controller->SetTrigger("landed");
                }

                // Idle/moving/attacking triggers based on velocity (only when grounded)
                if (navAgent->IsGrounded && !navAgent->IsJumping) {
                    // Check if entity has BehaviorTreeComponent with IsAttacking flag
                    bool isAttacking = false;
                    if (const auto* btComp = _registry.try_get<BehaviorTreeComponent>(entity)) {
                        isAttacking = btComp->IsAttacking;
                    }

                    constexpr float idleThreshold = 0.5f;

                    // Debug logging for animation state
                    static int animDebugCounter = 0;
                    animDebugCounter++;
                    if (animDebugCounter % 120 == 1) {
                        FSM_ANIM_LOG << "[ANIM] CurrentSpeed=" << navAgent->CurrentSpeed
                                     << " HasDest=" << navAgent->HasDestination
                                     << " Velocity=(" << navAgent->CurrentVelocity.x << "," << navAgent->CurrentVelocity.z << ")"
                                     << std::endl;
                    }

                    if (isAttacking) {
                        anim.Controller->SetTrigger("attacking");
                    } else if (navAgent->CurrentSpeed < idleThreshold) {
                        anim.Controller->SetTrigger("idle");
                    } else {
                        anim.Controller->SetTrigger("moving");
                    }
                }

                // Track previous jump state
                anim.WasJumping = navAgent->IsJumping;
            }
        }

        // Update movement disable timer
        if (anim.MovementDisabled && anim.MovementDisabledDuration > 0.0f) {
            anim.MovementDisabledTimer += deltaTime;
            if (anim.MovementDisabledTimer >= anim.MovementDisabledDuration) {
                anim.MovementDisabled = false;
                anim.MovementDisabledTimer = 0.0f;
            }
        }

        // Update entity position for spatial audio
        anim.EntityPosition = transform.GetPosition();

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

            // Update animation time with speed multiplier
            anim.Time += deltaTime * effectiveSpeed;
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
        } else if (useDirectionalBlend) {
            // Directional animation blending (4-way blend based on movement direction)
            // Load all 4 directional animations
            auto loadAnim = [&](const std::string& guid) -> std::shared_ptr<AnimationData> {
                if (guid.empty()) return nullptr;
                if (_loadedAnimations.find(guid) == _loadedAnimations.end()) {
                    auto data = scene->GetAnimation(guid);
                    if (data) _loadedAnimations[guid] = data;
                }
                return _loadedAnimations[guid];
            };

            auto forwardAnim = loadAnim(anim.DirectionalWalkForwardGuid);
            auto backAnim = loadAnim(anim.DirectionalWalkBackGuid);
            auto leftAnim = loadAnim(anim.DirectionalWalkLeftGuid);
            auto rightAnim = loadAnim(anim.DirectionalWalkRightGuid);

            if (!forwardAnim || !backAnim) continue;

            // Calculate blend weights based on direction angle
            // directionAngle: 0 = forward, ±180 = backward, +90 = right, -90 = left
            float forwardWeight = 0.0f, backWeight = 0.0f, leftWeight = 0.0f, rightWeight = 0.0f;

            // Use quadrant-based blending
            // Note: negative angle = right, positive angle = left (consistent in both quadrants)
            float absAngle = std::abs(directionAngle);
            if (absAngle <= 90.0f) {
                // Front half: blend forward with left/right
                float lateralFactor = absAngle / 90.0f;  // 0 at forward, 1 at side
                forwardWeight = 1.0f - lateralFactor;
                if (directionAngle < 0.0f && rightAnim) {
                    rightWeight = lateralFactor;
                } else if (directionAngle > 0.0f && leftAnim) {
                    leftWeight = lateralFactor;
                } else {
                    forwardWeight = 1.0f;  // No strafe anim, use forward only
                }
            } else {
                // Back half: blend backward with left/right (same direction mapping)
                float lateralFactor = (180.0f - absAngle) / 90.0f;  // 0 at backward, 1 at side
                backWeight = 1.0f - lateralFactor;
                if (directionAngle < 0.0f && rightAnim) {
                    rightWeight = lateralFactor;
                } else if (directionAngle > 0.0f && leftAnim) {
                    leftWeight = lateralFactor;
                } else {
                    backWeight = 1.0f;  // No strafe anim, use back only
                }
            }

            // Update animation time (use forward animation duration as reference)
            anim.Time += deltaTime * std::abs(effectiveSpeed);
            if (anim.Loop && anim.Time > forwardAnim->Duration) {
                anim.Time = fmod(anim.Time, forwardAnim->Duration);
            }

            // Sample all animations first, then blend
            struct BoneSample {
                glm::vec3 pos = glm::vec3(0.0f);
                glm::quat rot = glm::quat(1, 0, 0, 0);
                glm::vec3 scl = glm::vec3(1.0f);
            };

            // Calculate normalized time (0-1) based on forward animation
            float normalizedTime = fmod(anim.Time, forwardAnim->Duration) / forwardAnim->Duration;

            // Debug output - show which animations are blending
            static int dirAnimCounter = 0;
            dirAnimCounter++;
            if (dirAnimCounter % 30 == 1) {
                std::string blendInfo = "";
                if (forwardWeight > 0.01f) blendInfo += "FWD:" + std::to_string(int(forwardWeight * 100)) + "% ";
                if (backWeight > 0.01f) blendInfo += "BACK:" + std::to_string(int(backWeight * 100)) + "% ";
                if (leftWeight > 0.01f) blendInfo += "LEFT:" + std::to_string(int(leftWeight * 100)) + "% ";
                if (rightWeight > 0.01f) blendInfo += "RIGHT:" + std::to_string(int(rightWeight * 100)) + "% ";

                DIR_ANIM_LOG << "[DIR_ANIM] angle=" << int(directionAngle) << "° "
                             << blendInfo
                             << "normTime=" << std::fixed << std::setprecision(2) << normalizedTime
                             << (absAngle > 90.0f ? " [BACK]" : " [FRONT]") << std::endl;
            }

            auto sampleAnimation = [&](std::shared_ptr<AnimationData> animData) -> std::unordered_map<std::string, BoneSample> {
                std::unordered_map<std::string, BoneSample> samples;
                if (!animData) return samples;

                // Use normalized time so all animations are at same relative phase
                float localTime = normalizedTime * animData->Duration;
                for (const auto& channel : animData->Channels) {
                    BoneSample s;
                    s.pos = SamplePosition(channel.PositionKeys, localTime);
                    s.rot = SampleRotation(channel.RotationKeys, localTime);
                    s.scl = SampleScale(channel.ScaleKeys, localTime);
                    samples[channel.BoneName] = s;
                }
                return samples;
            };

            auto forwardSamples = sampleAnimation(forwardAnim);
            auto backSamples = sampleAnimation(backAnim);
            auto leftSamples = sampleAnimation(leftAnim);
            auto rightSamples = sampleAnimation(rightAnim);

            // Blend animations based on weights
            std::unordered_map<std::string, BoneSample> blended;

            // Helper to blend two samples (with quaternion hemisphere fix)
            auto blendSample = [](const BoneSample& a, const BoneSample& b, float t) -> BoneSample {
                BoneSample result;
                result.pos = glm::mix(a.pos, b.pos, t);

                // Ensure quaternions are on same hemisphere for shortest path slerp
                glm::quat bRot = b.rot;
                if (glm::dot(a.rot, bRot) < 0.0f) {
                    bRot = -bRot;  // Flip to same hemisphere
                }
                result.rot = glm::slerp(a.rot, bRot, t);

                result.scl = glm::mix(a.scl, b.scl, t);
                return result;
            };

            // Iterate over all bones from forward animation (primary)
            for (const auto& [boneName, forwardSample] : forwardSamples) {
                BoneSample result = forwardSample;
                float accumulatedWeight = forwardWeight;

                // Check if this is the root bone (hips) - preserve forward animation's position
                bool isRootBone = (boneName == "hips" || boneName == "Hips" ||
                                   boneName == "root" || boneName == "Root" ||
                                   boneName == "pelvis" || boneName == "Pelvis");

                // Blend with back animation
                if (backWeight > 0.0f && backSamples.find(boneName) != backSamples.end()) {
                    const BoneSample& backSample = backSamples.at(boneName);
                    if (accumulatedWeight > 0.0f) {
                        float t = backWeight / (accumulatedWeight + backWeight);
                        result = blendSample(result, backSample, t);
                    } else {
                        result = backSample;
                    }
                    accumulatedWeight += backWeight;
                }

                // Blend with left animation
                if (leftWeight > 0.0f && leftSamples.find(boneName) != leftSamples.end()) {
                    const BoneSample& leftSample = leftSamples.at(boneName);
                    if (accumulatedWeight > 0.0f) {
                        float t = leftWeight / (accumulatedWeight + leftWeight);
                        result = blendSample(result, leftSample, t);
                    } else {
                        result = leftSample;
                    }
                    accumulatedWeight += leftWeight;
                }

                // Blend with right animation
                if (rightWeight > 0.0f && rightSamples.find(boneName) != rightSamples.end()) {
                    const BoneSample& rightSample = rightSamples.at(boneName);
                    if (accumulatedWeight > 0.0f) {
                        float t = rightWeight / (accumulatedWeight + rightWeight);
                        result = blendSample(result, rightSample, t);
                    } else {
                        result = rightSample;
                    }
                    accumulatedWeight += rightWeight;
                }

                // For root bone, preserve forward animation's position to prevent mesh shifting
                if (isRootBone) {
                    result.pos = forwardSample.pos;
                }

                blended[boneName] = result;
            }

            // Apply blended transforms to bones
            for (const auto& [boneName, sample] : blended) {
                std::string meshBoneName = TryMatchBoneName(boneName, boneNameToIndex);
                if (meshBoneName.empty()) continue;

                const int boneIndex = boneNameToIndex[meshBoneName];

                glm::mat4 localTransform = glm::mat4(1.0f);
                localTransform = glm::translate(localTransform, sample.pos);
                localTransform = localTransform * glm::toMat4(sample.rot);
                localTransform = glm::scale(localTransform, sample.scl);

                localTransforms[boneIndex] = localTransform;
            }
        } else {
            // Normal animation (no transition, no directional blend)
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

            // Update animation time with speed multiplier
            anim.Time += deltaTime * effectiveSpeed;

            // Check for animation completion (forward direction)
            if (effectiveSpeed > 0.0f && anim.Time >= animData->Duration && !anim.HasCompletedOnce) {
                anim.HasCompletedOnce = true;
                FSM_ANIM_LOG << "[ANIM] Animation completed: " << anim.CurrentAnimationGuid
                             << " Trigger: " << anim.OnCompleteTrigger << std::endl;
                // Set trigger if specified
                if (!anim.OnCompleteTrigger.empty() && anim.Controller) {
                    anim.Controller->SetTrigger(anim.OnCompleteTrigger);
                }
            }

            // Handle looping in both directions
            if (anim.Loop) {
                if (anim.Time > animData->Duration) {
                    anim.Time = fmod(anim.Time, animData->Duration);
                    anim.HasCompletedOnce = false;
                } else if (anim.Time < 0.0f) {
                    // Wrap around to end when playing backward
                    anim.Time = animData->Duration + fmod(anim.Time, animData->Duration);
                    if (anim.Time >= animData->Duration) {
                        anim.Time = animData->Duration - 0.001f;
                    }
                }
            } else {
                // Clamp for non-looping animations
                anim.Time = glm::clamp(anim.Time, 0.0f, animData->Duration);
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
