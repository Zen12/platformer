#pragma once
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/skeleton_builder.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

// Ozz Skeleton Loader
// Converts Assimp bone hierarchy to ozz::animation::Skeleton
class OzzSkeletonLoader {
private:
    // Convert aiMatrix4x4 to glm::mat4
    static glm::mat4 AiToGlm(const aiMatrix4x4& m) {
        return glm::mat4(
            m.a1, m.b1, m.c1, m.d1,
            m.a2, m.b2, m.c2, m.d2,
            m.a3, m.b3, m.c3, m.d3,
            m.a4, m.b4, m.c4, m.d4
        );
    }

    // Extract transform from matrix
    static void DecomposeMatrix(const glm::mat4& matrix, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale) {
        translation = glm::vec3(matrix[3]);

        glm::vec3 col0(matrix[0]);
        glm::vec3 col1(matrix[1]);
        glm::vec3 col2(matrix[2]);

        scale.x = glm::length(col0);
        scale.y = glm::length(col1);
        scale.z = glm::length(col2);

        if (scale.x != 0) col0 /= scale.x;
        if (scale.y != 0) col1 /= scale.y;
        if (scale.z != 0) col2 /= scale.z;

        glm::mat3 rotMat(col0, col1, col2);
        rotation = glm::quat_cast(rotMat);
    }

    // Build raw skeleton from Assimp scene
    static bool BuildRawSkeleton(const aiScene* scene, const aiNode* node,
                                 const std::unordered_map<std::string, int>& boneMap,
                                 ozz::animation::offline::RawSkeleton::Joint& parent,
                                 int depth = 0) {
        std::string nodeName = node->mName.C_Str();
        bool isBone = boneMap.find(nodeName) != boneMap.end();

        if (isBone) {
            // Create joint
            ozz::animation::offline::RawSkeleton::Joint joint;
            joint.name = nodeName;

            // Get local transform
            glm::mat4 localTransform = AiToGlm(node->mTransformation);
            glm::vec3 translation, scale;
            glm::quat rotation;
            DecomposeMatrix(localTransform, translation, rotation, scale);

            // Convert to ozz format
            joint.transform.translation = ozz::math::Float3(translation.x, translation.y, translation.z);
            joint.transform.rotation = ozz::math::Quaternion(rotation.x, rotation.y, rotation.z, rotation.w);
            joint.transform.scale = ozz::math::Float3(scale.x, scale.y, scale.z);

            // Process children recursively
            for (unsigned int i = 0; i < node->mNumChildren; i++) {
                BuildRawSkeleton(scene, node->mChildren[i], boneMap, joint, depth + 1);
            }

            // Add to parent
            parent.children.push_back(joint);
        } else {
            // Not a bone, but check children (might have bone descendants)
            for (unsigned int i = 0; i < node->mNumChildren; i++) {
                BuildRawSkeleton(scene, node->mChildren[i], boneMap, parent, depth);
            }
        }

        return true;
    }

public:
    // Load skeleton from file (glb, fbx, etc.)
    static std::shared_ptr<ozz::animation::Skeleton> Load(const std::string& path) {
        Assimp::Importer importer;
        // Use SAME flags as mesh loader to ensure we get all bones
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices
        );

        if (!scene || !scene->mRootNode) {
            return nullptr;
        }

        // Collect all bone names from meshes
        std::unordered_map<std::string, int> boneNameToIndex;
        int boneIndex = 0;

        for (unsigned int meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++) {
            const aiMesh* mesh = scene->mMeshes[meshIdx];
            for (unsigned int boneIdx = 0; boneIdx < mesh->mNumBones; boneIdx++) {
                const aiBone* bone = mesh->mBones[boneIdx];
                std::string boneName = bone->mName.C_Str();
                if (boneNameToIndex.find(boneName) == boneNameToIndex.end()) {
                    boneNameToIndex[boneName] = boneIndex++;
                }
            }
        }

        if (boneNameToIndex.empty()) {
            return nullptr;
        }

        // Build raw skeleton
        ozz::animation::offline::RawSkeleton raw_skeleton;
        raw_skeleton.roots.resize(1); // Usually one root

        // Traverse scene hierarchy to build skeleton
        BuildRawSkeleton(scene, scene->mRootNode, boneNameToIndex, raw_skeleton.roots[0], 0);

        // Validate raw skeleton
        if (!raw_skeleton.Validate()) {
            return nullptr;
        }

        // Build runtime skeleton
        ozz::animation::offline::SkeletonBuilder builder;
        auto skeleton = builder(raw_skeleton);
        if (!skeleton) {
            return nullptr;
        }

        return std::make_shared<ozz::animation::Skeleton>(std::move(*skeleton));
    }
};
