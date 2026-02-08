#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "skinned_mesh_renderer_component_serialization.hpp"
#include "../../renderer/bounds.hpp"
#include "../../system/guid.hpp"

class SkinnedMeshRendererComponent final {
public:
    struct Guid MeshGuid;
    struct Guid MaterialGuid;

    std::vector<glm::mat4> BoneTransforms;
    std::vector<glm::mat4> LocalBoneTransforms;
    std::vector<std::string> BoneNames;
    std::vector<glm::mat4> BoneOffsets;
    std::vector<int> BoneParents;
    Bounds MeshBounds;

    explicit SkinnedMeshRendererComponent(const SkinnedMeshRendererComponentSerialization &serialization)
        : MeshGuid(serialization.MeshGuid), MaterialGuid(serialization.MaterialGuid) {
        BoneTransforms.resize(100, glm::mat4(1.0f));
    }
};
