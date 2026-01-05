#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "skinned_mesh_renderer_component_serialization.hpp"
#include "../../renderer/bounds.hpp"

class SkinnedMeshRendererComponent final {
public:
    std::string Guid;
    std::string MaterialGuid;

    // Bone transformation matrices (will be updated by animation system)
    std::vector<glm::mat4> BoneTransforms;

    // Local bone transforms (before hierarchy computation)
    std::vector<glm::mat4> LocalBoneTransforms;

    // Bone names (for mapping animation bone names to indices)
    std::vector<std::string> BoneNames;

    // Bone offset matrices (inverse bind pose - mesh space to bone space)
    std::vector<glm::mat4> BoneOffsets;

    // Bone hierarchy (parent index for each bone, -1 = root)
    std::vector<int> BoneParents;

    // Bounding box for frustum culling (populated on first render from Scene)
    Bounds MeshBounds;

    explicit SkinnedMeshRendererComponent(const SkinnedMeshRendererComponentSerialization &serialization)
        : Guid(serialization.MeshGuid), MaterialGuid(serialization.MaterialGuid) {
        // Initialize with identity matrices (will be populated when mesh is loaded)
        BoneTransforms.resize(100, glm::mat4(1.0f)); // Support up to 100 bones
    }
};
