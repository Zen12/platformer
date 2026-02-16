#define GLM_ENABLE_EXPERIMENTAL
#include "fsm_animation_system.hpp"
#include "../navmesh_agent/navmesh_agent_component.hpp"
#include "fsm_factory.hpp"
#include "fsm_asset.hpp"
#include "fsm_asset_yaml.hpp"
#include <iomanip>

#define DEBUG_FSM_ANIM_SYSTEM 0
#define DEBUG_VELOCITY_ANIM 0
#define DEBUG_JUMP_TRIGGER 0
#define DEBUG_DIRECTIONAL_ANIM 0

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

void FsmAnimationSystem::SampleAnimationToSoA(
    const std::shared_ptr<AnimationData>& animData,
    float time,
    BoneSoA& outBones,
    const std::unordered_map<std::string, int>& boneNameToIndex
) {
    if (!animData) return;

    const size_t boneCount = boneNameToIndex.size();
    outBones.Resize(boneCount);

    for (const auto& channel : animData->Channels) {
        auto it = boneNameToIndex.find(channel.BoneName);
        if (it == boneNameToIndex.end()) continue;

        const int idx = it->second;

        glm::vec3 pos = SamplePosition(channel.PositionKeys, time);
        glm::quat rot = SampleRotation(channel.RotationKeys, time);
        glm::vec3 scl = SampleScale(channel.ScaleKeys, time);

        outBones.PosX[idx] = pos.x;
        outBones.PosY[idx] = pos.y;
        outBones.PosZ[idx] = pos.z;
        outBones.RotX[idx] = rot.x;
        outBones.RotY[idx] = rot.y;
        outBones.RotZ[idx] = rot.z;
        outBones.RotW[idx] = rot.w;
        outBones.ScaleX[idx] = scl.x;
        outBones.ScaleY[idx] = scl.y;
        outBones.ScaleZ[idx] = scl.z;
    }
}

void FsmAnimationSystem::ConvertSoAToLocalTransforms(
    const BoneSoA& bones,
    std::vector<glm::mat4>& localTransforms
) {
    const size_t count = bones.Count;
    if (localTransforms.size() != count) {
        localTransforms.resize(count, glm::mat4(1.0f));
    }

    for (size_t i = 0; i < count; ++i) {
        glm::vec3 pos(bones.PosX[i], bones.PosY[i], bones.PosZ[i]);
        glm::quat rot(bones.RotW[i], bones.RotX[i], bones.RotY[i], bones.RotZ[i]);
        glm::vec3 scl(bones.ScaleX[i], bones.ScaleY[i], bones.ScaleZ[i]);

        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, pos);
        transform = transform * glm::toMat4(rot);
        transform = glm::scale(transform, scl);

        localTransforms[i] = transform;
    }
}

void FsmAnimationSystem::OnTick() {
    float deltaTime = 0.016f;

    for (const auto &[_, dt] : _deltaTimeView.each()) {
        deltaTime = dt.Delta;
    }

    auto scene = _scene.lock();
    if (!scene) return;

    static int velBasedCounter = 0;
    for (const auto &[entity, anim, skinnedMesh, transform] : View.each()) {
        const auto& blenderConfig = anim.Blender.GetConfig();

        float effectiveSpeed = (anim.StateAnimationSpeed >= 0.0f)
            ? anim.StateAnimationSpeed
            : blenderConfig.AnimationSpeed;

        bool useVelocitySpeed = blenderConfig.VelocityBasedSpeed && !anim.StateDisableVelocitySpeed;
        if (useVelocitySpeed) {
            velBasedCounter++;
            if (const auto* navAgent = _registry.try_get<NavmeshAgentComponent>(entity)) {
                const float speedRatio = navAgent->MaxSpeed > 0.0f
                    ? navAgent->CurrentSpeed / navAgent->MaxSpeed
                    : 0.0f;
                effectiveSpeed *= speedRatio * blenderConfig.VelocitySpeedScale;

                if (velBasedCounter % 2000 == 1) {
                    VELOCITY_ANIM_LOG << "[ANIM VEL] Speed=" << navAgent->CurrentSpeed
                              << " Ratio=" << speedRatio
                              << " EffectiveSpeed=" << effectiveSpeed << std::endl;
                }
            }
        }

        bool useDirectionalBlend = false;
        float directionAngle = 0.0f;
        bool isMoving = false;
        bool isGrounded = true;

        if (const auto* navAgent = _registry.try_get<NavmeshAgentComponent>(entity)) {
            constexpr float moveThreshold = 0.5f;
            isMoving = navAgent->CurrentSpeed > moveThreshold;
            isGrounded = navAgent->IsGrounded;
        }

        if (isMoving && isGrounded) {
            if (const auto* ikAim = _registry.try_get<IKAimComponent>(entity)) {
                if (ikAim->RotateCharacterToAim && ikAim->Enabled) {
                    directionAngle = ikAim->MovementDirectionAngle;
                    if (blenderConfig.Mode == AnimationBlendMode::Directional &&
                        !blenderConfig.DirectionalForwardGuid.IsEmpty() &&
                        !blenderConfig.DirectionalBackGuid.IsEmpty()) {
                        useDirectionalBlend = true;
                    } else {
                        float absAngle = std::abs(directionAngle);
                        if (absAngle > 90.0f) {
                            effectiveSpeed = -effectiveSpeed;
                        }
                    }
                }
            }
        }

        if (!anim.FsmGuid.IsEmpty() && !anim.Controller) {
            if (auto assetManager = scene->GetAssetManager().lock()) {
                try {
                    auto fsmAsset = assetManager->LoadAssetByGuid<FsmAsset>(anim.FsmGuid);
                    anim.SelfPtr = std::shared_ptr<FsmAnimationComponent>(&anim, [](FsmAnimationComponent*){});

                    std::shared_ptr<AudioManager> audioManager = nullptr;
                    if (auto am = scene->GetAudioManager().lock()) {
                        audioManager = am;
                    }

                    anim.Controller = FsmFactory::Create(
                        fsmAsset,
                        nullptr,
                        nullptr,
                        nullptr,
                        audioManager,
                        anim.SelfPtr
                    );
                } catch (const std::exception& e) {
                    std::cerr << "Failed to load FSM: " << anim.FsmGuid.ToString() << " - " << e.what() << std::endl;
                }
            }
        }

        if (anim.Controller) {
            if (auto* navAgent = _registry.try_get<NavmeshAgentComponent>(entity)) {
                if (navAgent->IsJumping && !anim.WasJumping) {
                    JUMP_LOG << "[JUMP] Triggering jump!" << std::endl;
                    anim.Controller->SetTrigger("jump");
                }
                if (navAgent->JustLanded) {
                    JUMP_LOG << "[JUMP] Triggering landed!" << std::endl;
                    anim.Controller->SetTrigger("landed");
                }

                if (navAgent->IsGrounded && !navAgent->IsJumping) {
                    bool isAttacking = false;
                    if (const auto* btComp = _registry.try_get<BehaviorTreeComponent>(entity)) {
                        isAttacking = btComp->IsAttacking;
                    }

                    constexpr float idleThreshold = 0.5f;
                    if (isAttacking) {
                        anim.Controller->SetTrigger("attacking");
                    } else if (navAgent->CurrentSpeed < idleThreshold) {
                        anim.Controller->SetTrigger("idle");
                    } else {
                        anim.Controller->SetTrigger("moving");
                    }
                }
                anim.WasJumping = navAgent->IsJumping;
            }
        }

        if (anim.MovementDisabled && anim.MovementDisabledDuration > 0.0f) {
            anim.MovementDisabledTimer += deltaTime;
            if (anim.MovementDisabledTimer >= anim.MovementDisabledDuration) {
                anim.MovementDisabled = false;
                anim.MovementDisabledTimer = 0.0f;
            }
        }

        anim.EntityPosition = transform.GetPosition();

        if (anim.Controller) {
            anim.Controller->Update();
        }

        if (anim.IsTransitioning) {
            anim.TransitionProgress += deltaTime;
            if (anim.TransitionProgress >= anim.TransitionDuration) {
                if (!anim.OnCompleteTrigger.empty() && anim.Controller) {
                    anim.Controller->SetTrigger(anim.OnCompleteTrigger);
                }
                FSM_ANIM_LOG << "[ANIM] Transition complete" << std::endl;
                anim.IsTransitioning = false;
                anim.CurrentAnimationGuid = anim.ToAnimationGuid;
                anim.TransitionProgress = 0.0f;
                anim.HasCompletedOnce = false;
            }
        }

        if (anim.CurrentAnimationGuid.IsEmpty()) continue;

        if (skinnedMesh.BoneNames.empty()) {
            scene->GetMesh(skinnedMesh.MeshGuid);
            skinnedMesh.BoneNames = scene->GetMeshBoneNames(skinnedMesh.MeshGuid);
            skinnedMesh.BoneOffsets = scene->GetMeshBoneOffsets(skinnedMesh.MeshGuid);
            skinnedMesh.BoneParents = scene->GetMeshBoneParents(skinnedMesh.MeshGuid);
        }

        std::unordered_map<std::string, int> boneNameToIndex;
        for (size_t i = 0; i < skinnedMesh.BoneNames.size(); i++) {
            boneNameToIndex[skinnedMesh.BoneNames[i]] = static_cast<int>(i);
        }

        if (skinnedMesh.LocalBoneTransforms.size() != skinnedMesh.BoneNames.size()) {
            skinnedMesh.LocalBoneTransforms.resize(skinnedMesh.BoneNames.size(), glm::mat4(1.0f));
        }

        auto loadAnimation = [&](const Guid& guid) -> std::shared_ptr<AnimationData> {
            if (guid.IsEmpty()) return nullptr;
            if (_loadedAnimations.find(guid) == _loadedAnimations.end()) {
                auto data = scene->GetAnimation(guid);
                if (data) _loadedAnimations[guid] = data;
            }
            return _loadedAnimations[guid];
        };

        if (anim.IsTransitioning) {
            auto fromAnim = loadAnimation(anim.FromAnimationGuid);
            auto toAnim = loadAnimation(anim.ToAnimationGuid);
            if (!fromAnim || !toAnim) continue;

            anim.Time += deltaTime * effectiveSpeed;
            if (anim.Loop && anim.Time > toAnim->Duration) {
                anim.Time = fmod(anim.Time, toAnim->Duration);
            }

            float blendFactor = anim.TransitionProgress / anim.TransitionDuration;
            blendFactor = glm::clamp(blendFactor, 0.0f, 1.0f);

            SampleAnimationToSoA(fromAnim, anim.Time, anim.CurrentBones, boneNameToIndex);
            SampleAnimationToSoA(toAnim, anim.Time, anim.TargetBones, boneNameToIndex);
            AnimationBlender::LerpSoA(anim.BlendedBones, anim.CurrentBones, anim.TargetBones, blendFactor);
            ConvertSoAToLocalTransforms(anim.BlendedBones, skinnedMesh.LocalBoneTransforms);

        } else if (useDirectionalBlend) {
            auto forwardAnim = loadAnimation(blenderConfig.DirectionalForwardGuid);
            auto backAnim = loadAnimation(blenderConfig.DirectionalBackGuid);
            auto leftAnim = loadAnimation(blenderConfig.DirectionalLeftGuid);
            auto rightAnim = loadAnimation(blenderConfig.DirectionalRightGuid);

            if (!forwardAnim || !backAnim) continue;

            anim.Time += deltaTime * std::abs(effectiveSpeed);
            if (anim.Loop && anim.Time > forwardAnim->Duration) {
                anim.Time = fmod(anim.Time, forwardAnim->Duration);
            }

            float normalizedTime = fmod(anim.Time, forwardAnim->Duration) / forwardAnim->Duration;

            BoneSoA forwardBones, backBones, leftBones, rightBones;
            SampleAnimationToSoA(forwardAnim, normalizedTime * forwardAnim->Duration, forwardBones, boneNameToIndex);
            SampleAnimationToSoA(backAnim, normalizedTime * backAnim->Duration, backBones, boneNameToIndex);

            BoneSoA* leftPtr = nullptr;
            BoneSoA* rightPtr = nullptr;
            if (leftAnim) {
                SampleAnimationToSoA(leftAnim, normalizedTime * leftAnim->Duration, leftBones, boneNameToIndex);
                leftPtr = &leftBones;
            }
            if (rightAnim) {
                SampleAnimationToSoA(rightAnim, normalizedTime * rightAnim->Duration, rightBones, boneNameToIndex);
                rightPtr = &rightBones;
            }

            AnimationBlender::BlendDirectionalSoA(
                anim.BlendedBones,
                forwardBones,
                backBones,
                leftPtr,
                rightPtr,
                directionAngle
            );
            ConvertSoAToLocalTransforms(anim.BlendedBones, skinnedMesh.LocalBoneTransforms);

        } else {
            auto animData = loadAnimation(anim.CurrentAnimationGuid);
            if (!animData) continue;

            anim.Time += deltaTime * effectiveSpeed;

            if (effectiveSpeed > 0.0f && anim.Time >= animData->Duration && !anim.HasCompletedOnce) {
                anim.HasCompletedOnce = true;
                if (!anim.OnCompleteTrigger.empty() && anim.Controller) {
                    anim.Controller->SetTrigger(anim.OnCompleteTrigger);
                }
            }

            if (anim.Loop) {
                if (anim.Time > animData->Duration) {
                    anim.Time = fmod(anim.Time, animData->Duration);
                    anim.HasCompletedOnce = false;
                } else if (anim.Time < 0.0f) {
                    anim.Time = animData->Duration + fmod(anim.Time, animData->Duration);
                    if (anim.Time >= animData->Duration) {
                        anim.Time = animData->Duration - 0.001f;
                    }
                }
            } else {
                anim.Time = glm::clamp(anim.Time, 0.0f, animData->Duration);
            }

            SampleAnimationToSoA(animData, anim.Time, anim.CurrentBones, boneNameToIndex);
            ConvertSoAToLocalTransforms(anim.CurrentBones, skinnedMesh.LocalBoneTransforms);
        }

        std::vector<glm::mat4> worldTransforms(skinnedMesh.BoneNames.size(), glm::mat4(1.0f));
        std::vector<bool> processed(skinnedMesh.BoneNames.size(), false);
        const auto& boneParents = skinnedMesh.BoneParents;
        const auto& localBoneTransforms = skinnedMesh.LocalBoneTransforms;

        std::function<void(int)> computeWorldTransform = [&](int boneIndex) {
            if (processed[boneIndex]) return;
            int parentIndex = boneParents[boneIndex];
            if (parentIndex == -1) {
                worldTransforms[boneIndex] = localBoneTransforms[boneIndex];
            } else {
                computeWorldTransform(parentIndex);
                worldTransforms[boneIndex] = worldTransforms[parentIndex] * localBoneTransforms[boneIndex];
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
