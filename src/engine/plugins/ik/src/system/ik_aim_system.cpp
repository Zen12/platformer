#define GLM_ENABLE_EXPERIMENTAL
#include "ik_aim_system.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>
#include <cmath>

#define DEBUG_IK_AIM 0

#if DEBUG_IK_AIM
#define IK_LOG if(1) std::cout
#else
#define IK_LOG if(0) std::cout
#endif

static int s_ikLogCounter = 0;

IKAimSystem::IKAimSystem(
    const TypeView& view,
    const TypeDeltaTime& deltaTimeView,
    const TypeCamera& cameraView,
    const TypeWindow& windowView,
    const std::weak_ptr<Scene>& scene,
    entt::registry& registry
) : ISystemView(view), _deltaTimeView(deltaTimeView), _cameraView(cameraView), _windowView(windowView), _scene(scene), _registry(registry) {
}

IKAimSystem::TwoBoneIKResult IKAimSystem::SolveTwoBoneIK(
    const glm::vec3& upperPos,
    const glm::vec3& lowerPos,
    const glm::vec3& endPos,
    const glm::vec3& targetPos,
    const glm::vec3& poleVector,
    float upperLength,
    float lowerLength
) {
    TwoBoneIKResult result;

    glm::vec3 toTarget = targetPos - upperPos;
    float targetDist = glm::length(toTarget);

    if (targetDist < 0.001f) {
        return result;
    }

    float totalLength = upperLength + lowerLength;
    float minLength = std::abs(upperLength - lowerLength);

    targetDist = glm::clamp(targetDist, minLength + 0.001f, totalLength - 0.001f);

    float a = upperLength;
    float b = lowerLength;
    float c = targetDist;

    float cosElbow = (a * a + b * b - c * c) / (2.0f * a * b);
    cosElbow = glm::clamp(cosElbow, -1.0f, 1.0f);

    float cosShoulder = (a * a + c * c - b * b) / (2.0f * a * c);
    cosShoulder = glm::clamp(cosShoulder, -1.0f, 1.0f);
    float shoulderAngle = std::acos(cosShoulder);

    glm::vec3 targetDir = glm::normalize(toTarget);

    glm::vec3 poleDir = poleVector - upperPos;
    poleDir = poleDir - glm::dot(poleDir, targetDir) * targetDir;
    if (glm::length(poleDir) > 0.001f) {
        poleDir = glm::normalize(poleDir);
    } else {
        poleDir = glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::vec3 perpDir = glm::cross(targetDir, poleDir);
    if (glm::length(perpDir) > 0.001f) {
        perpDir = glm::normalize(perpDir);
    }

    glm::vec3 upperDir = targetDir * std::cos(shoulderAngle) + poleDir * std::sin(shoulderAngle);

    glm::vec3 originalUpperDir = glm::normalize(lowerPos - upperPos);
    glm::vec3 originalLowerDir = glm::normalize(endPos - lowerPos);

    if (glm::length(glm::cross(originalUpperDir, upperDir)) > 0.001f) {
        result.UpperRotation = glm::rotation(originalUpperDir, upperDir);
    } else {
        result.UpperRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    glm::vec3 newLowerPos = upperPos + upperDir * upperLength;
    glm::vec3 newLowerDir = glm::normalize(targetPos - newLowerPos);

    glm::vec3 rotatedOriginalLowerDir = result.UpperRotation * originalLowerDir;
    if (glm::length(glm::cross(rotatedOriginalLowerDir, newLowerDir)) > 0.001f) {
        result.LowerRotation = glm::rotation(rotatedOriginalLowerDir, newLowerDir);
    } else {
        result.LowerRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    result.Solved = true;
    return result;
}

void IKAimSystem::ApplyTorsoRotation(
    std::vector<glm::mat4>& localTransforms,
    float yaw,
    float pitch,
    int spineIndex,
    int spine1Index,
    int chestIndex,
    float spineWeight,
    float spine1Weight,
    float chestWeight
) {
    auto applyPartialRotation = [&localTransforms](int boneIndex, float yawPart, float pitchPart) {
        if (boneIndex < 0 || boneIndex >= static_cast<int>(localTransforms.size())) return;

        glm::quat yawRot = glm::angleAxis(glm::radians(yawPart), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat pitchRot = glm::angleAxis(glm::radians(pitchPart), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat combinedRot = yawRot * pitchRot;

        glm::vec3 scale, translation, skew;
        glm::vec4 perspective;
        glm::quat rotation;
        glm::decompose(localTransforms[boneIndex], scale, rotation, translation, skew, perspective);

        rotation = combinedRot * rotation;

        glm::mat4 newTransform = glm::translate(glm::mat4(1.0f), translation);
        newTransform = newTransform * glm::toMat4(rotation);
        newTransform = glm::scale(newTransform, scale);

        localTransforms[boneIndex] = newTransform;
    };

    applyPartialRotation(spineIndex, yaw * spineWeight, pitch * spineWeight);
    applyPartialRotation(spine1Index, yaw * spine1Weight, pitch * spine1Weight);
    applyPartialRotation(chestIndex, yaw * chestWeight, pitch * chestWeight);
}

void IKAimSystem::RecomputeWorldTransforms(
    std::vector<glm::mat4>& localTransforms,
    std::vector<glm::mat4>& worldTransforms,
    const std::vector<int>& boneParents
) {
    size_t numBones = localTransforms.size();
    worldTransforms.resize(numBones, glm::mat4(1.0f));
    std::vector<bool> processed(numBones, false);

    std::function<void(int)> computeWorldTransform = [&](int boneIndex) {
        if (boneIndex < 0 || boneIndex >= static_cast<int>(numBones)) return;
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

    for (size_t i = 0; i < numBones; i++) {
        computeWorldTransform(static_cast<int>(i));
    }
}

glm::vec3 IKAimSystem::ScreenToWorldRay(
    const glm::vec2& screenPos,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::u16vec2& windowSize
) const {
    float x = (2.0f * screenPos.x) / static_cast<float>(windowSize.x) - 1.0f;
    float y = 1.0f - (2.0f * screenPos.y) / static_cast<float>(windowSize.y);

    glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

    glm::mat4 invProjection = glm::inverse(projection);
    glm::vec4 rayEye = invProjection * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    glm::mat4 invView = glm::inverse(view);
    glm::vec4 rayWorld = invView * rayEye;

    return glm::normalize(glm::vec3(rayWorld));
}

void IKAimSystem::OnTick() {
    s_ikLogCounter++;
    bool shouldLog = (s_ikLogCounter % 120 == 1); // Log every 2 seconds at 60fps

    float deltaTime = 0.016f;

    for (const auto& [_, dt] : _deltaTimeView.each()) {
        deltaTime = dt.Delta;
    }

    glm::mat4 cameraView = glm::mat4(1.0f);
    glm::mat4 cameraProjection = glm::mat4(1.0f);
    glm::vec3 cameraPosition = glm::vec3(0.0f);
    bool hasCamera = false;

    for (const auto& [_, camera, camTransform] : _cameraView.each()) {
        cameraView = camera.View;
        cameraProjection = camera.Projection;
        cameraPosition = camTransform.GetPosition();
        hasCamera = true;
    }

    if (shouldLog && !hasCamera) {
        IK_LOG << "[IK] WARNING: No camera found!" << std::endl;
    }

    glm::u16vec2 windowSize = glm::u16vec2(1280, 720);
    glm::vec2 mousePosition = glm::vec2(640, 360);

    for (const auto& [_, window] : _windowView.each()) {
        windowSize = window.Size;
    }

    auto scene = _scene.lock();
    if (!scene) {
        if (shouldLog) IK_LOG << "[IK] WARNING: Scene is null!" << std::endl;
        return;
    }

    if (auto inputSystem = scene->GetInputSystem().lock()) {
        mousePosition = inputSystem->GetMouseWindowPosition();
    }

    int entityCount = 0;
    for (const auto& [entity, ikAim, skinnedMesh, transform] : View.each()) {
        entityCount++;

        if (shouldLog) {
            IK_LOG << "[IK] Processing entity #" << entityCount
                   << " | Enabled=" << ikAim.Enabled
                   << " | BoneNames.size=" << skinnedMesh.BoneNames.size()
                   << " | LocalBoneTransforms.size=" << skinnedMesh.LocalBoneTransforms.size()
                   << std::endl;
        }

        if (!ikAim.Enabled) {
            if (shouldLog) IK_LOG << "[IK]   -> Skipping: IK not enabled" << std::endl;
            ikAim.CurrentBlend = 0.0f;
            continue;
        }

        // Disable IK during jumps - check if entity is airborne
        if (const auto* navAgent = _registry.try_get<NavmeshAgentComponent>(entity)) {
            if (!navAgent->IsGrounded) {
                if (shouldLog) IK_LOG << "[IK]   -> Skipping: Character is airborne (jumping)" << std::endl;
                // Smoothly fade out IK blend
                float blendFactor = 1.0f - std::exp(-ikAim.BlendSpeed * deltaTime);
                ikAim.CurrentBlend = glm::mix(ikAim.CurrentBlend, 0.0f, blendFactor);
                continue;
            }
        }

        if (skinnedMesh.BoneNames.empty()) {
            if (shouldLog) IK_LOG << "[IK]   -> Skipping: BoneNames empty" << std::endl;
            continue;
        }

        ikAim.CacheBoneIndices(skinnedMesh.BoneNames);

        if (shouldLog) {
            IK_LOG << "[IK]   Bone indices cached:"
                   << " Spine=" << ikAim.SpineIndex
                   << " Spine1=" << ikAim.Spine1Index
                   << " Chest=" << ikAim.ChestIndex
                   << " UpperArmR=" << ikAim.UpperArmRIndex
                   << " LowerArmR=" << ikAim.LowerArmRIndex
                   << " HandR=" << ikAim.HandRIndex
                   << " UpperArmL=" << ikAim.UpperArmLIndex
                   << " LowerArmL=" << ikAim.LowerArmLIndex
                   << " HandL=" << ikAim.HandLIndex
                   << std::endl;

            // Print first 20 bone names for debugging
            IK_LOG << "[IK]   Available bones: ";
            for (size_t i = 0; i < std::min(skinnedMesh.BoneNames.size(), size_t(20)); i++) {
                IK_LOG << "[" << i << "]=" << skinnedMesh.BoneNames[i] << " ";
            }
            IK_LOG << std::endl;
        }

        if (!ikAim.HasValidBoneIndices()) {
            if (shouldLog) IK_LOG << "[IK]   -> Skipping: Invalid bone indices (some are -1)" << std::endl;
            continue;
        }

        if (skinnedMesh.LocalBoneTransforms.size() != skinnedMesh.BoneNames.size()) {
            if (shouldLog) IK_LOG << "[IK]   -> Skipping: LocalBoneTransforms size mismatch ("
                                  << skinnedMesh.LocalBoneTransforms.size() << " != " << skinnedMesh.BoneNames.size() << ")" << std::endl;
            continue;
        }

        glm::vec3 entityPos = transform.GetPosition();

        glm::vec3 rayDir = ScreenToWorldRay(mousePosition, cameraView, cameraProjection, windowSize);
        glm::vec3 rayOrigin = cameraPosition;

        float characterHeight = entityPos.y + 1.5f;
        float t = (characterHeight - rayOrigin.y) / rayDir.y;

        if (t > 0.0f) {
            ikAim.AimTarget = rayOrigin + rayDir * t;
            ikAim.HasAimTarget = true;
        }

        // Character rotation to face aim target
        if (ikAim.RotateCharacterToAim && ikAim.HasAimTarget) {
            // Set external rotation control flag on NavmeshAgentComponent
            if (auto* navAgent = _registry.try_get<NavmeshAgentComponent>(entity)) {
                navAgent->ExternalRotationControl = true;

                // Only rotate when grounded
                if (navAgent->IsGrounded && !navAgent->JustLanded) {
                    // Calculate target yaw angle (horizontal rotation to face aim target)
                    glm::vec3 toAim = ikAim.AimTarget - entityPos;
                    toAim.y = 0.0f; // Ignore vertical component

                    if (glm::length(toAim) > 0.01f) {
                        float targetYaw = glm::degrees(std::atan2(toAim.x, toAim.z));

                        glm::vec3 currentRotation = transform.GetEulerRotation();
                        float currentYaw = currentRotation.y;

                        // Calculate shortest angle difference
                        float angleDiff = targetYaw - currentYaw;
                        while (angleDiff > 180.0f) angleDiff -= 360.0f;
                        while (angleDiff < -180.0f) angleDiff += 360.0f;

                        // Smooth rotation
                        float rotationStep = angleDiff * std::min(1.0f, ikAim.CharacterRotationSpeed * deltaTime);
                        currentRotation.y += rotationStep;
                        transform.SetEulerRotation(currentRotation);
                    }
                }

                // Calculate movement direction angle relative to facing
                // This helps the animation system know if character is moving forward/backward/sideways
                glm::vec3 velocity = navAgent->CurrentVelocity;
                velocity.y = 0.0f;

                if (glm::length(velocity) > 0.1f) {
                    // Calculate forward direction from Y euler rotation
                    // Forward is along negative Z in local space, rotated by Y rotation
                    float yawRad = glm::radians(transform.GetEulerRotation().y);
                    glm::vec3 facingDir = glm::vec3(std::sin(yawRad), 0.0f, std::cos(yawRad));

                    glm::vec3 moveDir = glm::normalize(velocity);

                    // Calculate angle between facing and movement
                    // Positive = moving right relative to facing, negative = moving left
                    float dotForward = glm::dot(facingDir, moveDir);
                    glm::vec3 cross = glm::cross(facingDir, moveDir);
                    float dotRight = cross.y; // Y component of cross product gives signed angle

                    // atan2 gives us the signed angle: 0 = forward, 180/-180 = backward, 90 = right, -90 = left
                    ikAim.MovementDirectionAngle = glm::degrees(std::atan2(dotRight, dotForward));
                } else {
                    ikAim.MovementDirectionAngle = 0.0f;
                }
            }
        }

        if (shouldLog) {
            IK_LOG << "[IK]   Mouse=" << mousePosition.x << "," << mousePosition.y
                   << " | EntityPos=" << entityPos.x << "," << entityPos.y << "," << entityPos.z
                   << " | AimTarget=" << ikAim.AimTarget.x << "," << ikAim.AimTarget.y << "," << ikAim.AimTarget.z
                   << " | HasAimTarget=" << ikAim.HasAimTarget
                   << " | t=" << t
                   << std::endl;
        }

        float targetBlend = ikAim.HasAimTarget ? ikAim.BlendWeight : 0.0f;
        float blendFactor = 1.0f - std::exp(-ikAim.BlendSpeed * deltaTime);
        ikAim.CurrentBlend = glm::mix(ikAim.CurrentBlend, targetBlend, blendFactor);

        if (shouldLog) {
            IK_LOG << "[IK]   BlendWeight=" << ikAim.BlendWeight
                   << " | BlendSpeed=" << ikAim.BlendSpeed
                   << " | CurrentBlend=" << ikAim.CurrentBlend
                   << std::endl;
        }

        if (ikAim.CurrentBlend < 0.001f) {
            if (shouldLog) IK_LOG << "[IK]   -> Skipping: CurrentBlend too low (" << ikAim.CurrentBlend << ")" << std::endl;
            continue;
        }

        if (shouldLog) IK_LOG << "[IK]   -> APPLYING IK! CurrentBlend=" << ikAim.CurrentBlend << std::endl;

        std::vector<glm::mat4> originalLocalTransforms = skinnedMesh.LocalBoneTransforms;

        glm::mat4 entityWorldMatrix = transform.GetModel();
        glm::mat4 invEntityWorld = glm::inverse(entityWorldMatrix);
        glm::vec3 localAimTarget = glm::vec3(invEntityWorld * glm::vec4(ikAim.AimTarget, 1.0f));

        // Compute recoil (applied per-hand below)
        float recoilT = 0.0f;
        if (ikAim.RecoilTimer > 0.0f) {
            ikAim.RecoilTimer -= deltaTime;
            recoilT = glm::clamp(ikAim.RecoilTimer / ikAim.RecoilDuration, 0.0f, 1.0f);
        }

        std::vector<glm::mat4> worldTransforms;
        RecomputeWorldTransforms(skinnedMesh.LocalBoneTransforms, worldTransforms, skinnedMesh.BoneParents);

        glm::vec3 chestPos = glm::vec3(worldTransforms[ikAim.ChestIndex][3]);
        glm::vec3 toTarget = localAimTarget - chestPos;

        float yaw = glm::degrees(std::atan2(toTarget.x, toTarget.z));
        float horizontalDist = std::sqrt(toTarget.x * toTarget.x + toTarget.z * toTarget.z);
        float pitch = glm::degrees(std::atan2(toTarget.y - chestPos.y, horizontalDist));

        yaw = glm::clamp(yaw, -ikAim.TorsoMaxYaw, ikAim.TorsoMaxYaw);
        pitch = glm::clamp(pitch, -ikAim.TorsoMaxPitch, ikAim.TorsoMaxPitch);

        float blendedYaw = yaw * ikAim.CurrentBlend;
        float blendedPitch = pitch * ikAim.CurrentBlend;

        ApplyTorsoRotation(
            skinnedMesh.LocalBoneTransforms,
            blendedYaw,
            blendedPitch,
            ikAim.SpineIndex,
            ikAim.Spine1Index,
            ikAim.ChestIndex,
            ikAim.SpineWeight,
            ikAim.Spine1Weight,
            ikAim.ChestWeight
        );

        RecomputeWorldTransforms(skinnedMesh.LocalBoneTransforms, worldTransforms, skinnedMesh.BoneParents);

        // Apply right arm IK - compute absolute orientation to override animation
        {
            int upperIndex = ikAim.UpperArmRIndex;
            int lowerIndex = ikAim.LowerArmRIndex;
            int handIndex = ikAim.HandRIndex;
            int parentIndex = skinnedMesh.BoneParents[upperIndex];

            if (upperIndex >= 0 && lowerIndex >= 0 && handIndex >= 0) {
                glm::vec3 upperPos = glm::vec3(worldTransforms[upperIndex][3]);
                glm::vec3 lowerPos = glm::vec3(worldTransforms[lowerIndex][3]);
                glm::vec3 handPos = glm::vec3(worldTransforms[handIndex][3]);

                float upperLength = glm::length(lowerPos - upperPos);
                float lowerLength = glm::length(handPos - lowerPos);
                float totalLength = upperLength + lowerLength;

                glm::vec3 rightArmBaseTarget = localAimTarget + ikAim.RightHandOffset;
                glm::vec3 targetPos = rightArmBaseTarget
                    + (ikAim.RecoilHand == 0 ? recoilT * ikAim.RecoilOffsetRight : glm::vec3(0.0f));
                glm::vec3 toTarget = targetPos - upperPos;
                float targetDist = glm::length(toTarget);

                // Clamp target distance to reachable range
                targetDist = glm::clamp(targetDist, 0.01f, totalLength * 0.999f);
                glm::vec3 targetDir = glm::normalize(toTarget);
                targetPos = upperPos + targetDir * targetDist;

                // Law of cosines to find shoulder angle
                float cosShoulder = (upperLength * upperLength + targetDist * targetDist - lowerLength * lowerLength)
                                   / (2.0f * upperLength * targetDist);
                cosShoulder = glm::clamp(cosShoulder, -1.0f, 1.0f);
                float shoulderAngle = std::acos(cosShoulder);

                // Build coordinate frame for arm plane using pole vector
                glm::vec3 poleTarget = upperPos + ikAim.RightElbowHint;
                glm::vec3 poleDir = poleTarget - upperPos;
                poleDir = poleDir - glm::dot(poleDir, targetDir) * targetDir;
                if (glm::length(poleDir) > 0.001f) {
                    poleDir = glm::normalize(poleDir);
                } else {
                    poleDir = glm::vec3(0.0f, -1.0f, 0.0f);
                }

                // Compute new elbow position
                glm::vec3 elbowDir = targetDir * std::cos(shoulderAngle) + poleDir * std::sin(shoulderAngle);
                glm::vec3 newElbowPos = upperPos + elbowDir * upperLength;

                // Get the original local transforms for blending
                glm::vec3 origUpperScale, origUpperTranslation, origUpperSkew;
                glm::vec4 origUpperPerspective;
                glm::quat origUpperLocalRot;
                glm::decompose(originalLocalTransforms[upperIndex], origUpperScale, origUpperLocalRot, origUpperTranslation, origUpperSkew, origUpperPerspective);

                // Compute rotations for upper arm relative to the ORIGINAL (animated) pose
                glm::vec3 origUpperDir = glm::normalize(lowerPos - upperPos);
                glm::vec3 newUpperDir = glm::normalize(newElbowPos - upperPos);

                glm::quat upperDeltaRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                if (glm::length(glm::cross(origUpperDir, newUpperDir)) > 0.0001f) {
                    upperDeltaRot = glm::rotation(origUpperDir, newUpperDir);
                }

                // Get parent world rotation
                glm::vec3 parentScale, parentTranslation, parentSkew;
                glm::vec4 parentPerspective;
                glm::quat parentWorldRot;
                if (parentIndex >= 0) {
                    glm::decompose(worldTransforms[parentIndex], parentScale, parentWorldRot, parentTranslation, parentSkew, parentPerspective);
                } else {
                    parentWorldRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                }

                // Apply rotation: convert world delta to local delta
                glm::quat localDeltaRot = glm::inverse(parentWorldRot) * upperDeltaRot * parentWorldRot;
                glm::quat blendedLocalDelta = glm::slerp(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), localDeltaRot, ikAim.CurrentBlend);
                glm::quat newUpperLocalRot = blendedLocalDelta * origUpperLocalRot;

                glm::mat4 newTransform = glm::translate(glm::mat4(1.0f), origUpperTranslation);
                newTransform = newTransform * glm::toMat4(newUpperLocalRot);
                newTransform = glm::scale(newTransform, origUpperScale);
                skinnedMesh.LocalBoneTransforms[upperIndex] = newTransform;

                // Recompute world transforms after upper arm change
                RecomputeWorldTransforms(skinnedMesh.LocalBoneTransforms, worldTransforms, skinnedMesh.BoneParents);

                // Lower arm rotation
                glm::vec3 actualElbowPos = glm::vec3(worldTransforms[lowerIndex][3]);
                glm::vec3 actualHandPos = glm::vec3(worldTransforms[handIndex][3]);

                glm::vec3 origLowerDir = glm::normalize(actualHandPos - actualElbowPos);
                glm::vec3 newLowerDir = glm::normalize(targetPos - actualElbowPos);

                glm::quat lowerDeltaRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                if (glm::length(glm::cross(origLowerDir, newLowerDir)) > 0.0001f) {
                    lowerDeltaRot = glm::rotation(origLowerDir, newLowerDir);
                }

                // Get upper arm (parent) world rotation
                glm::vec3 upperScale, upperTranslation, upperSkew;
                glm::vec4 upperPerspective;
                glm::quat upperWorldRot;
                glm::decompose(worldTransforms[upperIndex], upperScale, upperWorldRot, upperTranslation, upperSkew, upperPerspective);

                // Get original lower arm local rotation
                glm::vec3 origLowerScale, origLowerTranslation, origLowerSkew;
                glm::vec4 origLowerPerspective;
                glm::quat origLowerLocalRot;
                glm::decompose(originalLocalTransforms[lowerIndex], origLowerScale, origLowerLocalRot, origLowerTranslation, origLowerSkew, origLowerPerspective);

                // Apply rotation to lower arm
                glm::quat lowerLocalDelta = glm::inverse(upperWorldRot) * lowerDeltaRot * upperWorldRot;
                glm::quat blendedLowerDelta = glm::slerp(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), lowerLocalDelta, ikAim.CurrentBlend);
                glm::quat newLowerLocalRot = blendedLowerDelta * origLowerLocalRot;

                glm::mat4 lowerTransform = glm::translate(glm::mat4(1.0f), origLowerTranslation);
                lowerTransform = lowerTransform * glm::toMat4(newLowerLocalRot);
                lowerTransform = glm::scale(lowerTransform, origLowerScale);
                skinnedMesh.LocalBoneTransforms[lowerIndex] = lowerTransform;

                if (shouldLog) {
                    IK_LOG << "[IK]   Right arm IK: target=" << targetPos.x << "," << targetPos.y << "," << targetPos.z
                           << " upperLen=" << upperLength << " lowerLen=" << lowerLength << std::endl;
                }
            }
        }

        RecomputeWorldTransforms(skinnedMesh.LocalBoneTransforms, worldTransforms, skinnedMesh.BoneParents);

        // When right hand is recoiling, save the non-recoiled hand position for the left arm grip.
        // We do this by temporarily solving right arm without recoil, recording the hand pos,
        // then the actual recoiled solve above is already baked into the bone transforms.
        glm::vec3 rightHandGripPos = glm::vec3(worldTransforms[ikAim.HandRIndex][3]);
        if (ikAim.RecoilHand == 0 && recoilT > 0.0f) {
            // Re-solve right arm without recoil to find where hand would be
            int rUpperIdx = ikAim.UpperArmRIndex;
            int rLowerIdx = ikAim.LowerArmRIndex;
            int rHandIdx = ikAim.HandRIndex;
            // Save current (recoiled) bone transforms
            glm::mat4 savedUpper = skinnedMesh.LocalBoneTransforms[rUpperIdx];
            glm::mat4 savedLower = skinnedMesh.LocalBoneTransforms[rLowerIdx];
            // Restore original and re-solve without recoil
            skinnedMesh.LocalBoneTransforms[rUpperIdx] = originalLocalTransforms[rUpperIdx];
            skinnedMesh.LocalBoneTransforms[rLowerIdx] = originalLocalTransforms[rLowerIdx];
            // Recompute with torso rotation but original arm pose
            std::vector<glm::mat4> tempWorld;
            RecomputeWorldTransforms(skinnedMesh.LocalBoneTransforms, tempWorld, skinnedMesh.BoneParents);
            glm::vec3 upperPos = glm::vec3(tempWorld[rUpperIdx][3]);
            glm::vec3 lowerPos = glm::vec3(tempWorld[rLowerIdx][3]);
            glm::vec3 handPos = glm::vec3(tempWorld[rHandIdx][3]);
            float upperLen = glm::length(lowerPos - upperPos);
            float lowerLen = glm::length(handPos - lowerPos);
            float totalLen = upperLen + lowerLen;
            glm::vec3 baseTarget = localAimTarget + ikAim.RightHandOffset;
            glm::vec3 toBase = baseTarget - upperPos;
            float baseDist = glm::clamp(glm::length(toBase), 0.01f, totalLen * 0.999f);
            rightHandGripPos = upperPos + glm::normalize(toBase) * baseDist;
            // Restore the recoiled bone transforms
            skinnedMesh.LocalBoneTransforms[rUpperIdx] = savedUpper;
            skinnedMesh.LocalBoneTransforms[rLowerIdx] = savedLower;
        }

        // Apply left arm IK (reaches toward right hand grip point)
        {
            int upperIndex = ikAim.UpperArmLIndex;
            int lowerIndex = ikAim.LowerArmLIndex;
            int handIndex = ikAim.HandLIndex;
            int parentIndex = skinnedMesh.BoneParents[upperIndex];

            glm::vec3 leftArmTarget = rightHandGripPos + ikAim.LeftHandGripPoint
                + (ikAim.RecoilHand == 1 ? recoilT * ikAim.RecoilOffsetLeft : glm::vec3(0.0f));

            if (upperIndex >= 0 && lowerIndex >= 0 && handIndex >= 0) {
                glm::vec3 upperPos = glm::vec3(worldTransforms[upperIndex][3]);
                glm::vec3 lowerPos = glm::vec3(worldTransforms[lowerIndex][3]);
                glm::vec3 handPos = glm::vec3(worldTransforms[handIndex][3]);

                float upperLength = glm::length(lowerPos - upperPos);
                float lowerLength = glm::length(handPos - lowerPos);
                float totalLength = upperLength + lowerLength;

                glm::vec3 toTarget = leftArmTarget - upperPos;
                float targetDist = glm::length(toTarget);

                targetDist = glm::clamp(targetDist, 0.01f, totalLength * 0.999f);
                glm::vec3 targetDir = glm::normalize(toTarget);
                glm::vec3 targetPos = upperPos + targetDir * targetDist;

                float cosShoulder = (upperLength * upperLength + targetDist * targetDist - lowerLength * lowerLength)
                                   / (2.0f * upperLength * targetDist);
                cosShoulder = glm::clamp(cosShoulder, -1.0f, 1.0f);
                float shoulderAngle = std::acos(cosShoulder);

                glm::vec3 poleTarget = upperPos + ikAim.LeftElbowHint;
                glm::vec3 poleDir = poleTarget - upperPos;
                poleDir = poleDir - glm::dot(poleDir, targetDir) * targetDir;
                if (glm::length(poleDir) > 0.001f) {
                    poleDir = glm::normalize(poleDir);
                } else {
                    poleDir = glm::vec3(0.0f, -1.0f, 0.0f);
                }

                glm::vec3 elbowDir = targetDir * std::cos(shoulderAngle) + poleDir * std::sin(shoulderAngle);
                glm::vec3 newElbowPos = upperPos + elbowDir * upperLength;

                // Get original upper arm local rotation for blending
                glm::vec3 origUpperScale, origUpperTranslation, origUpperSkew;
                glm::vec4 origUpperPerspective;
                glm::quat origUpperLocalRot;
                glm::decompose(originalLocalTransforms[upperIndex], origUpperScale, origUpperLocalRot, origUpperTranslation, origUpperSkew, origUpperPerspective);

                glm::vec3 origUpperDir = glm::normalize(lowerPos - upperPos);
                glm::vec3 newUpperDir = glm::normalize(newElbowPos - upperPos);

                glm::quat upperDeltaRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                if (glm::length(glm::cross(origUpperDir, newUpperDir)) > 0.0001f) {
                    upperDeltaRot = glm::rotation(origUpperDir, newUpperDir);
                }

                glm::vec3 parentScale, parentTranslation, parentSkew;
                glm::vec4 parentPerspective;
                glm::quat parentWorldRot;
                if (parentIndex >= 0) {
                    glm::decompose(worldTransforms[parentIndex], parentScale, parentWorldRot, parentTranslation, parentSkew, parentPerspective);
                } else {
                    parentWorldRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                }

                glm::quat localDeltaRot = glm::inverse(parentWorldRot) * upperDeltaRot * parentWorldRot;
                glm::quat blendedLocalDelta = glm::slerp(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), localDeltaRot, ikAim.CurrentBlend);
                glm::quat newUpperLocalRot = blendedLocalDelta * origUpperLocalRot;

                glm::mat4 upperTransform = glm::translate(glm::mat4(1.0f), origUpperTranslation);
                upperTransform = upperTransform * glm::toMat4(newUpperLocalRot);
                upperTransform = glm::scale(upperTransform, origUpperScale);
                skinnedMesh.LocalBoneTransforms[upperIndex] = upperTransform;

                RecomputeWorldTransforms(skinnedMesh.LocalBoneTransforms, worldTransforms, skinnedMesh.BoneParents);

                // Lower arm
                glm::vec3 actualElbowPos = glm::vec3(worldTransforms[lowerIndex][3]);
                glm::vec3 actualHandPos = glm::vec3(worldTransforms[handIndex][3]);

                glm::vec3 origLowerDir = glm::normalize(actualHandPos - actualElbowPos);
                glm::vec3 newLowerDir = glm::normalize(targetPos - actualElbowPos);

                glm::quat lowerDeltaRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
                if (glm::length(glm::cross(origLowerDir, newLowerDir)) > 0.0001f) {
                    lowerDeltaRot = glm::rotation(origLowerDir, newLowerDir);
                }

                glm::vec3 upperScale, upperTranslation, upperSkew;
                glm::vec4 upperPerspective;
                glm::quat upperWorldRot;
                glm::decompose(worldTransforms[upperIndex], upperScale, upperWorldRot, upperTranslation, upperSkew, upperPerspective);

                // Get original lower arm local rotation
                glm::vec3 origLowerScale, origLowerTranslation, origLowerSkew;
                glm::vec4 origLowerPerspective;
                glm::quat origLowerLocalRot;
                glm::decompose(originalLocalTransforms[lowerIndex], origLowerScale, origLowerLocalRot, origLowerTranslation, origLowerSkew, origLowerPerspective);

                glm::quat lowerLocalDelta = glm::inverse(upperWorldRot) * lowerDeltaRot * upperWorldRot;
                glm::quat blendedLowerDelta = glm::slerp(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), lowerLocalDelta, ikAim.CurrentBlend);
                glm::quat newLowerLocalRot = blendedLowerDelta * origLowerLocalRot;

                glm::mat4 lowerTransform = glm::translate(glm::mat4(1.0f), origLowerTranslation);
                lowerTransform = lowerTransform * glm::toMat4(newLowerLocalRot);
                lowerTransform = glm::scale(lowerTransform, origLowerScale);
                skinnedMesh.LocalBoneTransforms[lowerIndex] = lowerTransform;
            }
        }

        RecomputeWorldTransforms(skinnedMesh.LocalBoneTransforms, worldTransforms, skinnedMesh.BoneParents);

        skinnedMesh.WorldBoneTransforms = worldTransforms;

        for (size_t i = 0; i < skinnedMesh.BoneTransforms.size(); i++) {
            if (i < worldTransforms.size() && i < skinnedMesh.BoneOffsets.size()) {
                skinnedMesh.BoneTransforms[i] = worldTransforms[i] * skinnedMesh.BoneOffsets[i];
            } else {
                skinnedMesh.BoneTransforms[i] = glm::mat4(1.0f);
            }
        }

        if (shouldLog) IK_LOG << "[IK]   -> IK APPLIED SUCCESSFULLY" << std::endl;
    }

    if (shouldLog && entityCount == 0) {
        IK_LOG << "[IK] WARNING: No entities with IKAimComponent + SkinnedMeshRendererComponent + TransformComponentV2 found!" << std::endl;
    }
}
