#pragma once
#include <string>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

struct BoneAttachmentComponent {
    std::string ParentTag;
    std::string BoneName;
    glm::vec3 PositionOffset = glm::vec3(0.0f);
    glm::vec3 RotationOffset = glm::vec3(0.0f);

    // Cached at runtime
    int CachedBoneIndex = -1;
    entt::entity CachedParentEntity = entt::null;
    bool CacheResolved = false;
};
