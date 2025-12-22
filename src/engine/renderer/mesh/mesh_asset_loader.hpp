#pragma once

#include "mesh.hpp"
#include "../../asset/loaders/asset_loader.hpp"
#include <memory>
#include <string>
#include <vector>

// Intermediate structure for mesh data loaded from 3D model files
// Supports GLB (recommended), FBX, OBJ, and other Assimp-supported formats
// GLB provides 5-50x faster loading compared to FBX
struct MeshData {
    std::vector<float> Vertices;      // Vertex data (x,y,z) or (x,y,z,u,v) if HasTexCoords
    std::vector<uint32_t> Indices;    // Triangle indices
    bool HasTexCoords;                 // Whether UV coordinates are present
};

template<>
class AssetLoader<MeshData> : public AssetLoaderBase<MeshData, AssetLoader<MeshData>> {
public:
    static void Init();
    static MeshData LoadImpl(const std::string& path);
};
