#include "mesh_asset_loader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <functional>
#include <unordered_map>

#define DEBUG_MESH_LOADER 0

#if DEBUG_MESH_LOADER
#include <iostream>
#define MESH_LOG if(1) std::cout
#else
#define MESH_LOG if(0) std::cout
#endif

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
    MESH_LOG << "[MESH_LOADER] Starting to load: " << path << std::endl;
    MESH_LOG << "[MESH_LOADER] Path length: " << path.length() << " bytes" << std::endl;

#ifdef __EMSCRIPTEN__
    MESH_LOG << "[MESH_LOADER] Running in EMSCRIPTEN/WASM environment" << std::endl;
#else
    MESH_LOG << "[MESH_LOADER] Running in native environment" << std::endl;
#endif

    // Check if file exists (basic check)
    std::ifstream fileCheck(path, std::ios::binary);
    if (!fileCheck.good()) {
        std::cerr << "[MESH_LOADER] ERROR: Cannot open file at path: " << path << std::endl;
        std::cerr << "[MESH_LOADER] File does not exist or is not accessible" << std::endl;
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    }
    fileCheck.seekg(0, std::ios::end);
    std::streamsize fileSize = fileCheck.tellg();
    fileCheck.close();
    MESH_LOG << "[MESH_LOADER] File exists, size: " << fileSize << " bytes" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    MESH_LOG << "[MESH_LOADER] Creating Assimp::Importer..." << std::endl;
    Assimp::Importer importer;

#ifdef __EMSCRIPTEN__
    // Log available importers in WASM build
    MESH_LOG << "[MESH_LOADER] Checking available importers..." << std::endl;

    // Get extension list
    aiString extensionList;
    importer.GetExtensionList(extensionList);
    MESH_LOG << "[MESH_LOADER] Supported extensions: " << extensionList.C_Str() << std::endl;

    // Check if GLB is supported
    bool glbSupported = importer.IsExtensionSupported(".glb");
    bool gltfSupported = importer.IsExtensionSupported(".gltf");
    MESH_LOG << "[MESH_LOADER] GLB format supported: " << (glbSupported ? "YES" : "NO") << std::endl;
    MESH_LOG << "[MESH_LOADER] GLTF format supported: " << (gltfSupported ? "YES" : "NO") << std::endl;

    if (!glbSupported) {
        std::cerr << "[MESH_LOADER] ERROR: GLB importer is NOT compiled into this Assimp build!" << std::endl;
        std::cerr << "[MESH_LOADER] You need to rebuild Assimp with ASSIMP_BUILD_GLTF_IMPORTER=ON" << std::endl;
        std::cerr << "[MESH_LOADER] Available formats: " << extensionList.C_Str() << std::endl;
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    }
#endif

    // Use minimal flags for WASM to avoid crashes
    unsigned int importFlags;
#ifdef __EMSCRIPTEN__
    // WASM: Try with NO flags first to see if it loads at all
    importFlags = 0;
    MESH_LOG << "[MESH_LOADER] Using WASM-safe flags: NONE (0) - raw import only" << std::endl;
#else
    // Native: Use optimized flags
    importFlags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;
    MESH_LOG << "[MESH_LOADER] Using native flags: aiProcess_Triangulate | aiProcess_JoinIdenticalVertices" << std::endl;
#endif

    const aiScene* scene = nullptr;

#ifdef __EMSCRIPTEN__
    // WASM: Load file into memory first, then use ReadFileFromMemory
    // This avoids potential WASM filesystem issues with Assimp
    MESH_LOG << "[MESH_LOADER] WASM: Loading file into memory buffer..." << std::endl;

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "[MESH_LOADER] ERROR: Failed to open file for memory loading" << std::endl;
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cerr << "[MESH_LOADER] ERROR: Failed to read file into buffer" << std::endl;
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    }
    file.close();

    MESH_LOG << "[MESH_LOADER] Loaded " << size << " bytes into memory" << std::endl;

    // Detect format from file extension
    std::string hint = path.substr(path.find_last_of('.') + 1);
    MESH_LOG << "[MESH_LOADER] File format hint: " << hint << std::endl;
    MESH_LOG << "[MESH_LOADER] Buffer address: " << static_cast<const void*>(buffer.data()) << std::endl;
    MESH_LOG << "[MESH_LOADER] Buffer size: " << buffer.size() << std::endl;
    MESH_LOG << "[MESH_LOADER] Import flags: " << importFlags << std::endl;

    MESH_LOG << "[MESH_LOADER] About to call ReadFileFromMemory..." << std::endl;
    MESH_LOG << "[MESH_LOADER] This may crash - checking console for errors..." << std::endl;

    // Set a very conservative read limit for WASM
    importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);

    MESH_LOG << "[MESH_LOADER] Calling ReadFileFromMemory NOW..." << std::endl;

    scene = importer.ReadFileFromMemory(buffer.data(), buffer.size(), importFlags, hint.c_str());

    MESH_LOG << "[MESH_LOADER] !!! ReadFileFromMemory completed successfully !!!" << std::endl;

    // Check error even if no exception was thrown
    if (!scene) {
        std::cerr << "[MESH_LOADER] ERROR: ReadFileFromMemory returned null" << std::endl;
        std::cerr << "[MESH_LOADER] Assimp error: " << importer.GetErrorString() << std::endl;
        std::cerr.flush();
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    }

    MESH_LOG << "[MESH_LOADER] Scene loaded successfully!" << std::endl;
#else
    // Native: Use direct file reading
    MESH_LOG << "[MESH_LOADER] Calling importer.ReadFile..." << std::endl;

    try {
        scene = importer.ReadFile(path, importFlags);
    } catch (const std::exception& e) {
        std::cerr << "[MESH_LOADER] EXCEPTION during ReadFile: " << e.what() << std::endl;
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    } catch (...) {
        std::cerr << "[MESH_LOADER] UNKNOWN EXCEPTION during ReadFile" << std::endl;
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    }
#endif

    MESH_LOG << "[MESH_LOADER] ReadFile returned, scene pointer: " << (scene ? "valid" : "null") << std::endl;

    auto loadTime = std::chrono::high_resolution_clock::now();
    auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(loadTime - start).count();
    MESH_LOG << "[MESH_LOADER] ReadFile took " << loadDuration << "ms" << std::endl;

    if (!scene) {
        std::cerr << "[MESH_LOADER] ERROR: Scene is null" << std::endl;
        std::cerr << "[MESH_LOADER] Assimp error: " << importer.GetErrorString() << std::endl;
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    }

    if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cerr << "[MESH_LOADER] ERROR: Scene has INCOMPLETE flag" << std::endl;
        std::cerr << "[MESH_LOADER] Assimp error: " << importer.GetErrorString() << std::endl;
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    }

    if (!scene->mRootNode) {
        std::cerr << "[MESH_LOADER] ERROR: Scene has no root node" << std::endl;
        std::cerr << "[MESH_LOADER] Assimp error: " << importer.GetErrorString() << std::endl;
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    }

    MESH_LOG << "[MESH_LOADER] Assimp loaded scene successfully" << std::endl;
    MESH_LOG << "[MESH_LOADER] Number of meshes in scene: " << scene->mNumMeshes << std::endl;

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
    MESH_LOG << "[MESH_LOADER] Pre-calculated sizes - vertices: " << totalVertices << ", indices: " << totalIndices << std::endl;
    vertices.reserve(totalVertices * 5); // 5 floats per vertex (x,y,z,u,v) max
    indices.reserve(totalIndices);
    if (hasSkeleton) {
        boneWeights.reserve(totalVertices * 4); // 4 weights per vertex
        boneIndices.reserve(totalVertices * 4); // 4 bone indices per vertex
        MESH_LOG << "[MESH_LOADER] Mesh has skeleton, reserved bone data" << std::endl;
    }

    auto processStart = std::chrono::high_resolution_clock::now();
    MESH_LOG << "[MESH_LOADER] Starting mesh processing..." << std::endl;

    for (unsigned int meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++) {
        const aiMesh* mesh = scene->mMeshes[meshIdx];
        MESH_LOG << "[MESH_LOADER] Processing mesh " << meshIdx << "/" << scene->mNumMeshes
                  << " - vertices: " << mesh->mNumVertices << ", faces: " << mesh->mNumFaces << std::endl;

        if (!(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)) {
            MESH_LOG << "[MESH_LOADER] Skipping non-triangle mesh" << std::endl;
            continue;
        }

        if (meshIdx == 0 && mesh->mTextureCoords[0] != nullptr) {
            hasTexCoords = true;
        }

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            // Import vertices as-is from Blender (no coordinate transformation)
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);

            if (hasTexCoords && mesh->mTextureCoords[0]) {
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            const aiFace& face = mesh->mFaces[i];
            // Import indices as-is from Blender (no winding order change)
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j] + indexOffset);
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

                for (unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++) {
                    const aiBone* bone = mesh->mBones[boneIdx];
                    boneNames.push_back(bone->mName.C_Str());

                    // Convert aiMatrix4x4 to glm::mat4 (Assimp uses row-major, GLM uses column-major)
                    // Import as-is from Blender (no coordinate transformation)
                    const aiMatrix4x4& m = bone->mOffsetMatrix;
                    glm::mat4 offset(
                        m.a1, m.b1, m.c1, m.d1,
                        m.a2, m.b2, m.c2, m.d2,
                        m.a3, m.b3, m.c3, m.d3,
                        m.a4, m.b4, m.c4, m.d4
                    );
                    boneOffsets.push_back(offset);
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

        MESH_LOG << "  Built bone hierarchy: ";
        int rootBones = 0;
        for (int parent : boneParents) {
            if (parent == -1) rootBones++;
        }
        MESH_LOG << rootBones << " root bones" << std::endl;
    }

    if (vertices.empty() || indices.empty()) {
        std::cerr << "[MESH_LOADER] ERROR: No mesh data found in " << path << std::endl;
        std::cerr << "[MESH_LOADER] vertices.size()=" << vertices.size() << ", indices.size()=" << indices.size() << std::endl;
        return MeshData{{}, {}, false, {}, {}, false, {}, {}, {}};
    }

    MESH_LOG << "[MESH_LOADER] Mesh processing complete - final data sizes:" << std::endl;
    MESH_LOG << "[MESH_LOADER]   vertices: " << vertices.size() << ", indices: " << indices.size() << std::endl;

    auto processEnd = std::chrono::high_resolution_clock::now();
    auto processDuration = std::chrono::duration_cast<std::chrono::milliseconds>(processEnd - processStart).count();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(processEnd - start).count();

    MESH_LOG << "MESH_LOADER: Loaded " << path << std::endl;
    MESH_LOG << "  Total vertices: " << (hasTexCoords ? vertices.size() / 5 : vertices.size() / 3) << std::endl;
    MESH_LOG << "  Total indices: " << indices.size() << std::endl;
    MESH_LOG << "  Has texture coords: " << (hasTexCoords ? "yes" : "no") << std::endl;
    MESH_LOG << "  Has skeleton: " << (hasSkeleton ? "yes" : "no") << std::endl;
    if (hasSkeleton) {
        MESH_LOG << "  Bone data: " << (boneWeights.size() / 4) << " vertices with bone weights" << std::endl;
        MESH_LOG << "  Bone count: " << boneNames.size() << std::endl;
        if (!boneNames.empty()) {
            MESH_LOG << "  First bones: ";
            for (size_t i = 0; i < std::min<size_t>(5, boneNames.size()); i++) {
                MESH_LOG << boneNames[i];
                if (i < std::min<size_t>(5, boneNames.size()) - 1) MESH_LOG << ", ";
            }
            MESH_LOG << std::endl;
        }
    }
    MESH_LOG << "  Load time: " << loadDuration << "ms, Process time: " << processDuration << "ms, Total: " << totalDuration << "ms" << std::endl;

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

        MESH_LOG << "  Bounds: X[" << minX << ", " << maxX << "] "
                  << "Y[" << minY << ", " << maxY << "] "
                  << "Z[" << minZ << ", " << maxZ << "]" << std::endl;
    }

    return MeshData{vertices, indices, hasTexCoords, boneWeights, boneIndices, hasSkeleton, boneNames, boneOffsets, boneParents};
}
