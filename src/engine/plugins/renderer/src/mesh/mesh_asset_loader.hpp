#pragma once

#include "mesh.hpp"
#include "loaders/asset_loader.hpp"
#include "../bounds.hpp"
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

// Intermediate structure for mesh data loaded from 3D model files
// Supports GLB (recommended), FBX, OBJ, and other Assimp-supported formats
// GLB provides 5-50x faster loading compared to FBX
struct MeshData {
    std::vector<float> Vertices;      // Vertex data (x,y,z) or (x,y,z,u,v) if HasTexCoords
    std::vector<uint32_t> Indices;    // Triangle indices
    bool HasTexCoords;                 // Whether UV coordinates are present

    // Skeletal animation data
    std::vector<float> BoneWeights;    // 4 weights per vertex (w1,w2,w3,w4)
    std::vector<int> BoneIndices;      // 4 bone indices per vertex (i1,i2,i3,i4)
    bool HasSkeleton;                  // Whether bone data is present
    std::vector<std::string> BoneNames; // Bone names (index matches bone index)
    std::vector<glm::mat4> BoneOffsets; // Inverse bind pose matrices (mesh space -> bone space)
    std::vector<int> BoneParents; // Parent bone index for each bone (-1 = no parent/root)

    // Bounding box (axis-aligned in mesh local space)
    Bounds MeshBounds;
};

template<>
class AssetLoader<MeshData> : public AssetLoaderBase<MeshData, AssetLoader<MeshData>> {
public:
    static void Init();
    static MeshData LoadImpl(const std::string& path);
};
