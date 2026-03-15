#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "bone_attachment_component.hpp"
#include "esc/esc_core.hpp"
#include "transform/transform_component.hpp"
#include "skinned_mesh_renderer/skinned_mesh_renderer_component.hpp"
#include "tag/tag_component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <entt/entt.hpp>

class BoneAttachmentSystem final : public ISystemView<BoneAttachmentComponent, TransformComponentV2> {
private:
    entt::registry& _registry;

public:
    explicit BoneAttachmentSystem(const TypeView& view, entt::registry& registry)
        : ISystemView(view), _registry(registry) {}

    void OnTick() override {
        for (auto [entity, attachment, transform] : View.each()) {
            if (!attachment.CacheResolved) {
                auto tagView = _registry.view<TagComponent, SkinnedMeshRendererComponent, TransformComponentV2>();
                for (auto [tagEntity, tag, skinned, parentTransform] : tagView.each()) {
                    if (tag.GetTag() == attachment.ParentTag) {
                        attachment.CachedParentEntity = tagEntity;
                        for (size_t i = 0; i < skinned.BoneNames.size(); ++i) {
                            if (skinned.BoneNames[i] == attachment.BoneName) {
                                attachment.CachedBoneIndex = static_cast<int>(i);
                                break;
                            }
                        }
                        break;
                    }
                }
                attachment.CacheResolved = (attachment.CachedBoneIndex >= 0);
            }

            if (attachment.CachedParentEntity == entt::null || attachment.CachedBoneIndex < 0) continue;

            auto* parentSkinned = _registry.try_get<SkinnedMeshRendererComponent>(attachment.CachedParentEntity);
            auto* parentTransform = _registry.try_get<TransformComponentV2>(attachment.CachedParentEntity);
            if (!parentSkinned || !parentTransform) continue;
            if (parentSkinned->WorldBoneTransforms.empty()) continue;
            if (attachment.CachedBoneIndex >= static_cast<int>(parentSkinned->WorldBoneTransforms.size())) continue;

            glm::mat4 entityModel = parentTransform->GetModel();
            glm::mat4 boneWorld = parentSkinned->WorldBoneTransforms[attachment.CachedBoneIndex];

            // Build offset matrix
            glm::mat4 offsetMatrix = glm::mat4(1.0f);
            offsetMatrix = glm::translate(offsetMatrix, attachment.PositionOffset);
            offsetMatrix = glm::rotate(offsetMatrix, glm::radians(attachment.RotationOffset.z), glm::vec3(0, 0, 1));
            offsetMatrix = glm::rotate(offsetMatrix, glm::radians(attachment.RotationOffset.y), glm::vec3(0, 1, 0));
            offsetMatrix = glm::rotate(offsetMatrix, glm::radians(attachment.RotationOffset.x), glm::vec3(1, 0, 0));

            glm::mat4 finalTransform = entityModel * boneWorld * offsetMatrix;

            // Extract position
            glm::vec3 position = glm::vec3(finalTransform[3]);

            // Extract rotation (ZYX euler angles to match TransformComponentV2::GetModel)
            glm::vec3 col0 = glm::vec3(finalTransform[0]);
            glm::vec3 col1 = glm::vec3(finalTransform[1]);
            glm::vec3 col2 = glm::vec3(finalTransform[2]);
            float sx = glm::length(col0);
            float sy = glm::length(col1);
            float sz = glm::length(col2);

            glm::mat4 rotMat = glm::mat4(1.0f);
            if (sx > 0.0001f) rotMat[0] = glm::vec4(col0 / sx, 0.0f);
            if (sy > 0.0001f) rotMat[1] = glm::vec4(col1 / sy, 0.0f);
            if (sz > 0.0001f) rotMat[2] = glm::vec4(col2 / sz, 0.0f);

            float rz, ry, rx;
            glm::extractEulerAngleZYX(rotMat, rz, ry, rx);

            transform.SetPosition(position);
            transform.SetEulerRotation(glm::vec3(glm::degrees(rx), glm::degrees(ry), glm::degrees(rz)));
        }
    }
};
