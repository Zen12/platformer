#include "mesh_asset_loader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

// Mesh loader using Assimp library
// Recommended format: GLB (5-50x faster than FBX, optimized for real-time)
// Also supports: FBX, OBJ, COLLADA, and 40+ other formats
//
// Coordinate system conversion:
// - Blender exports Y-up (vertical) by default
// - Engine uses Y-forward (depth), Z-up (vertical)
// - Loader automatically converts coordinates
//
// Performance optimizations:
// - Pre-allocates vectors to avoid reallocations
// - Reverses winding order for correct face culling
// - Includes timing measurements for profiling

void AssetLoader<MeshData>::Init() {
}

MeshData AssetLoader<MeshData>::LoadImpl(const std::string& path) {
    auto start = std::chrono::high_resolution_clock::now();

    Assimp::Importer importer;

    // Optimized flags for faster loading - removed OptimizeMeshes which is slow for large meshes
    // Removed aiProcess_FlipUVs as GLB files from Blender have correct UV orientation
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices
    );

    auto loadTime = std::chrono::high_resolution_clock::now();
    auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(loadTime - start).count();

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return MeshData{{}, {}, false};
    }

    // Pre-calculate total sizes to avoid vector reallocations
    size_t totalVertices = 0;
    size_t totalIndices = 0;
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        if (scene->mMeshes[i]->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) {
            totalVertices += scene->mMeshes[i]->mNumVertices;
            for (unsigned int j = 0; j < scene->mMeshes[i]->mNumFaces; j++) {
                totalIndices += scene->mMeshes[i]->mFaces[j].mNumIndices;
            }
        }
    }

    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    bool hasTexCoords = false;
    uint32_t indexOffset = 0;

    // Reserve capacity to avoid reallocations (major performance boost for large meshes)
    vertices.reserve(totalVertices * 5); // 5 floats per vertex (x,y,z,u,v) max
    indices.reserve(totalIndices);

    auto processStart = std::chrono::high_resolution_clock::now();

    for (unsigned int meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++) {
        const aiMesh* mesh = scene->mMeshes[meshIdx];

        if (!(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)) {
            continue;
        }

        if (meshIdx == 0 && mesh->mTextureCoords[0] != nullptr) {
            hasTexCoords = true;
        }

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            // Convert from FBX Y-up coordinates to renderer coordinates (Y-forward, Z-up)
            // FBX:      X=right, Y=up,      Z=back
            // Renderer: X=right, Y=forward, Z=up
            // Transformation: Renderer(x,y,z) = FBX(x, -z, -y) with negated Y to flip vertical
            const float fbx_x = mesh->mVertices[i].x;
            const float fbx_y = mesh->mVertices[i].y;
            const float fbx_z = mesh->mVertices[i].z;

            vertices.push_back(fbx_x);   // X stays the same
            vertices.push_back(fbx_z);  // FBX's -Z (forward) becomes renderer's Y (forward)
            vertices.push_back(fbx_y);  // FBX's -Y becomes renderer's Z (up) - negated to fix upside-down

            if (hasTexCoords && mesh->mTextureCoords[0]) {
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            const aiFace& face = mesh->mFaces[i];
            // Reverse winding order because coordinate transformation flips handedness
            // Original: 0, 1, 2 -> Reversed: 0, 2, 1
            if (face.mNumIndices == 3) {
                indices.push_back(face.mIndices[0] + indexOffset);
                indices.push_back(face.mIndices[2] + indexOffset);
                indices.push_back(face.mIndices[1] + indexOffset);
            } else {
                // Non-triangle faces (shouldn't happen with aiProcess_Triangulate)
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j] + indexOffset);
                }
            }
        }

        indexOffset += mesh->mNumVertices;
    }

    if (vertices.empty() || indices.empty()) {
        std::cerr << "ERROR::MESH_LOADER: No mesh data found in " << path << std::endl;
        return MeshData{{}, {}, false};
    }

    auto processEnd = std::chrono::high_resolution_clock::now();
    auto processDuration = std::chrono::duration_cast<std::chrono::milliseconds>(processEnd - processStart).count();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(processEnd - start).count();

    std::cout << "MESH_LOADER: Loaded " << path << std::endl;
    std::cout << "  Total vertices: " << (hasTexCoords ? vertices.size() / 5 : vertices.size() / 3) << std::endl;
    std::cout << "  Total indices: " << indices.size() << std::endl;
    std::cout << "  Has texture coords: " << (hasTexCoords ? "yes" : "no") << std::endl;
    std::cout << "  Load time: " << loadDuration << "ms, Process time: " << processDuration << "ms, Total: " << totalDuration << "ms" << std::endl;

    if (!vertices.empty()) {
        float minX = vertices[0], maxX = vertices[0];
        float minY = hasTexCoords ? vertices[1] : vertices[1], maxY = hasTexCoords ? vertices[1] : vertices[1];
        float minZ = hasTexCoords ? vertices[2] : vertices[2], maxZ = hasTexCoords ? vertices[2] : vertices[2];

        int stride = hasTexCoords ? 5 : 3;
        for (size_t i = 0; i < vertices.size(); i += stride) {
            minX = std::min(minX, vertices[i]);
            maxX = std::max(maxX, vertices[i]);
            minY = std::min(minY, vertices[i + 1]);
            maxY = std::max(maxY, vertices[i + 1]);
            minZ = std::min(minZ, vertices[i + 2]);
            maxZ = std::max(maxZ, vertices[i + 2]);
        }

        std::cout << "  Bounds: X[" << minX << ", " << maxX << "] "
                  << "Y[" << minY << ", " << maxY << "] "
                  << "Z[" << minZ << ", " << maxZ << "]" << std::endl;
    }

    return MeshData{vertices, indices, hasTexCoords};
}
