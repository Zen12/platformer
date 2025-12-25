#include "mesh_asset_loader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <functional>
#include <unordered_map>

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
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
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
    std::vector<float> boneWeights;
    std::vector<int> boneIndices;
    std::vector<std::string> boneNames;
    std::vector<glm::mat4> boneOffsets;
    std::vector<int> boneParents;
    bool hasTexCoords = false;
    bool hasSkeleton = false;
    uint32_t indexOffset = 0;

    // Check if any mesh has bones
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        if (scene->mMeshes[i]->mNumBones > 0) {
            hasSkeleton = true;
            break;
        }
    }

    // Reserve capacity to avoid reallocations (major performance boost for large meshes)
    vertices.reserve(totalVertices * 5); // 5 floats per vertex (x,y,z,u,v) max
    indices.reserve(totalIndices);
    if (hasSkeleton) {
        boneWeights.reserve(totalVertices * 4); // 4 weights per vertex
        boneIndices.reserve(totalVertices * 4); // 4 bone indices per vertex
    }

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
            // Transformation: Renderer(x,y,z) = FBX(x, z, y)
            const float fbx_x = mesh->mVertices[i].x;
            const float fbx_y = mesh->mVertices[i].y;
            const float fbx_z = mesh->mVertices[i].z;

            vertices.push_back(fbx_x);   // X stays the same
            vertices.push_back(fbx_z);   // Z becomes Y
            vertices.push_back(fbx_y);   // Y becomes Z

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

        // Process bone data if present
        if (hasSkeleton && mesh->mNumBones > 0) {
            // Initialize bone data for all vertices in this mesh (4 bones per vertex)
            std::vector<std::vector<std::pair<int, float>>> vertexBones(mesh->mNumVertices);

            // Store bone names and offset matrices on first mesh with bones
            if (boneNames.empty()) {
                boneNames.reserve(mesh->mNumBones);
                boneOffsets.reserve(mesh->mNumBones);

                // Coordinate transformation matrix: Blender (Y-up) -> Engine (Z-up, Y-forward)
                // Swaps Y and Z axes: Engine(x,y,z) = Blender(x,z,y)
                const glm::mat4 coordTransform(
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f
                );

                for (unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++) {
                    const aiBone* bone = mesh->mBones[boneIdx];
                    boneNames.push_back(bone->mName.C_Str());

                    // Convert aiMatrix4x4 to glm::mat4 (Assimp uses row-major, GLM uses column-major)
                    const aiMatrix4x4& m = bone->mOffsetMatrix;
                    glm::mat4 offsetBlender(
                        m.a1, m.b1, m.c1, m.d1,
                        m.a2, m.b2, m.c2, m.d2,
                        m.a3, m.b3, m.c3, m.d3,
                        m.a4, m.b4, m.c4, m.d4
                    );

                    // Transform offset matrix to engine coordinates: T * offset * T^-1
                    // Since T is symmetric (swaps Y and Z), T^-1 = T
                    glm::mat4 offsetEngine = coordTransform * offsetBlender * coordTransform;
                    boneOffsets.push_back(offsetEngine);
                }
            }

            // Read bone weights from mesh
            for (unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++) {
                const aiBone* bone = mesh->mBones[boneIdx];

                for (unsigned int weightIdx = 0; weightIdx < bone->mNumWeights; weightIdx++) {
                    const aiVertexWeight& weight = bone->mWeights[weightIdx];
                    vertexBones[weight.mVertexId].push_back({static_cast<int>(boneIdx), weight.mWeight});
                }
            }

            // Write bone data for each vertex (up to 4 bones per vertex)
            for (unsigned int vertexIdx = 0; vertexIdx < mesh->mNumVertices; vertexIdx++) {
                auto& bones = vertexBones[vertexIdx];

                // Sort by weight (descending) and keep top 4
                std::sort(bones.begin(), bones.end(),
                    [](const auto& a, const auto& b) { return a.second > b.second; });

                // Add bone indices and weights (4 per vertex)
                for (int i = 0; i < 4; i++) {
                    if (i < static_cast<int>(bones.size())) {
                        boneIndices.push_back(bones[i].first);
                        boneWeights.push_back(bones[i].second);
                    } else {
                        boneIndices.push_back(0);
                        boneWeights.push_back(0.0f);
                    }
                }
            }
        } else if (hasSkeleton) {
            // No bones for this mesh, but other meshes have bones - add default data
            for (unsigned int vertexIdx = 0; vertexIdx < mesh->mNumVertices; vertexIdx++) {
                for (int i = 0; i < 4; i++) {
                    boneIndices.push_back(0);
                    boneWeights.push_back(0.0f);
                }
            }
        }

        indexOffset += mesh->mNumVertices;
    }

    // Build bone hierarchy if skeleton exists
    if (hasSkeleton && !boneNames.empty()) {
        boneParents.resize(boneNames.size(), -1); // Initialize all as root (-1 = no parent)

        // Create bone name to index map for quick lookup
        std::unordered_map<std::string, int> boneNameToIndex;
        for (size_t i = 0; i < boneNames.size(); i++) {
            boneNameToIndex[boneNames[i]] = static_cast<int>(i);
        }

        // Recursive function to traverse node hierarchy
        std::function<void(const aiNode*, int)> processNode = [&](const aiNode* node, int parentIndex) {
            // Check if this node is a bone
            auto it = boneNameToIndex.find(node->mName.C_Str());
            int currentIndex = parentIndex;
            if (it != boneNameToIndex.end()) {
                currentIndex = it->second;
                boneParents[currentIndex] = parentIndex;
            }

            // Process children
            for (unsigned int i = 0; i < node->mNumChildren; i++) {
                processNode(node->mChildren[i], currentIndex);
            }
        };

        // Start from root node
        processNode(scene->mRootNode, -1);

        std::cout << "  Built bone hierarchy: ";
        int rootBones = 0;
        for (int parent : boneParents) {
            if (parent == -1) rootBones++;
        }
        std::cout << rootBones << " root bones" << std::endl;
    }

    if (vertices.empty() || indices.empty()) {
        std::cerr << "ERROR::MESH_LOADER: No mesh data found in " << path << std::endl;
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    }

    auto processEnd = std::chrono::high_resolution_clock::now();
    auto processDuration = std::chrono::duration_cast<std::chrono::milliseconds>(processEnd - processStart).count();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(processEnd - start).count();

    std::cout << "MESH_LOADER: Loaded " << path << std::endl;
    std::cout << "  Total vertices: " << (hasTexCoords ? vertices.size() / 5 : vertices.size() / 3) << std::endl;
    std::cout << "  Total indices: " << indices.size() << std::endl;
    std::cout << "  Has texture coords: " << (hasTexCoords ? "yes" : "no") << std::endl;
    std::cout << "  Has skeleton: " << (hasSkeleton ? "yes" : "no") << std::endl;
    if (hasSkeleton) {
        std::cout << "  Bone data: " << (boneWeights.size() / 4) << " vertices with bone weights" << std::endl;
        std::cout << "  Bone count: " << boneNames.size() << std::endl;
        if (!boneNames.empty()) {
            std::cout << "  First bones: ";
            for (size_t i = 0; i < std::min<size_t>(5, boneNames.size()); i++) {
                std::cout << boneNames[i];
                if (i < std::min<size_t>(5, boneNames.size()) - 1) std::cout << ", ";
            }
            std::cout << std::endl;
        }
    }
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

    return MeshData{vertices, indices, hasTexCoords, boneWeights, boneIndices, hasSkeleton, boneNames, boneOffsets, boneParents};
}
