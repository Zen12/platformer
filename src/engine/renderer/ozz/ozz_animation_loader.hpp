#pragma once
#include <ozz/animation/runtime/animation.h>
#include <ozz/animation/offline/raw_animation.h>
#include <ozz/animation/offline/animation_builder.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

// Ozz Animation Loader
// Converts Assimp animation data to ozz::animation::Animation
class OzzAnimationLoader {
private:
    // Convert aiQuaternion to glm::quat
    static glm::quat AiToGlmQuat(const aiQuaternion& q) {
        return glm::quat(q.w, q.x, q.y, q.z);
    }

    // Convert aiVector3D to glm::vec3
    static glm::vec3 AiToGlmVec3(const aiVector3D& v) {
        return glm::vec3(v.x, v.y, v.z);
    }

public:
    // Load animation from file
    // animationIndex: which animation to load from the file (default 0)
    static std::shared_ptr<ozz::animation::Animation> Load(const std::string& path,
                                                           const ozz::animation::Skeleton& skeleton,
                                                           int animationIndex = 0) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices
        );

        if (!scene || !scene->mRootNode) {
            return nullptr;
        }

        if (scene->mNumAnimations == 0) {
            return nullptr;
        }

        if (animationIndex >= static_cast<int>(scene->mNumAnimations)) {
            return nullptr;
        }

        const aiAnimation* aiAnim = scene->mAnimations[animationIndex];

        // Calculate duration in seconds
        double ticksPerSecond = aiAnim->mTicksPerSecond != 0.0 ? aiAnim->mTicksPerSecond : 25.0;
        float duration = static_cast<float>(aiAnim->mDuration / ticksPerSecond);

        // Build raw animation
        ozz::animation::offline::RawAnimation raw_animation;
        raw_animation.duration = duration;
        raw_animation.tracks.resize(skeleton.num_joints());

        // Create map from bone name to skeleton joint index
        std::unordered_map<std::string, int> jointNameToIndex;
        for (int i = 0; i < skeleton.num_joints(); ++i) {
            jointNameToIndex[skeleton.joint_names()[i]] = i;
        }

        // Process each animation channel (bone)
        for (unsigned int channelIdx = 0; channelIdx < aiAnim->mNumChannels; channelIdx++) {
            const aiNodeAnim* channel = aiAnim->mChannels[channelIdx];
            std::string boneName = channel->mNodeName.C_Str();

            // Try to match bone name to skeleton joint
            // Handle mixamorig: prefix if needed
            std::string cleanBoneName = boneName;
            const std::string mixamoPrefix = "mixamorig:";
            if (boneName.substr(0, mixamoPrefix.length()) == mixamoPrefix) {
                cleanBoneName = boneName.substr(mixamoPrefix.length());
            }

            int jointIndex = -1;
            auto it = jointNameToIndex.find(boneName);
            if (it != jointNameToIndex.end()) {
                jointIndex = it->second;
            } else {
                it = jointNameToIndex.find(cleanBoneName);
                if (it != jointNameToIndex.end()) {
                    jointIndex = it->second;
                }
            }

            if (jointIndex == -1) {
                // Bone not found in skeleton, skip
                continue;
            }

            auto& track = raw_animation.tracks[jointIndex];

            // Convert position keys
            track.translations.reserve(channel->mNumPositionKeys);
            for (unsigned int i = 0; i < channel->mNumPositionKeys; i++) {
                const aiVectorKey& key = channel->mPositionKeys[i];
                float time = static_cast<float>(key.mTime / ticksPerSecond);
                glm::vec3 pos = AiToGlmVec3(key.mValue);

                // Mixamo animations are in centimeters, convert to meters
                pos *= 0.01f;

                ozz::animation::offline::RawAnimation::TranslationKey ozz_key;
                ozz_key.time = time;
                ozz_key.value = ozz::math::Float3(pos.x, pos.y, pos.z);
                track.translations.push_back(ozz_key);
            }

            // Convert rotation keys
            track.rotations.reserve(channel->mNumRotationKeys);
            for (unsigned int i = 0; i < channel->mNumRotationKeys; i++) {
                const aiQuatKey& key = channel->mRotationKeys[i];
                float time = static_cast<float>(key.mTime / ticksPerSecond);
                glm::quat rot = AiToGlmQuat(key.mValue);

                ozz::animation::offline::RawAnimation::RotationKey ozz_key;
                ozz_key.time = time;
                ozz_key.value = ozz::math::Quaternion(rot.x, rot.y, rot.z, rot.w);
                track.rotations.push_back(ozz_key);
            }

            // Convert scale keys
            track.scales.reserve(channel->mNumScalingKeys);
            for (unsigned int i = 0; i < channel->mNumScalingKeys; i++) {
                const aiVectorKey& key = channel->mScalingKeys[i];
                float time = static_cast<float>(key.mTime / ticksPerSecond);
                glm::vec3 scale = AiToGlmVec3(key.mValue);

                ozz::animation::offline::RawAnimation::ScaleKey ozz_key;
                ozz_key.time = time;
                ozz_key.value = ozz::math::Float3(scale.x, scale.y, scale.z);
                track.scales.push_back(ozz_key);
            }
        }

        // Validate raw animation
        if (!raw_animation.Validate()) {
            return nullptr;
        }

        // Build runtime animation
        ozz::animation::offline::AnimationBuilder builder;
        auto animation = builder(raw_animation);
        if (!animation) {
            return nullptr;
        }

        return std::make_shared<ozz::animation::Animation>(std::move(*animation));
    }
};
