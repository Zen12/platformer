#include "animation_asset_loader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define DEBUG_ANIM_LOADER 1

#if DEBUG_ANIM_LOADER
#include <iostream>
#define ANIM_LOG if(1) std::cout
#else
#define ANIM_LOG if(0) std::cout
#endif

void AssetLoader<AnimationData>::Init() {
}

AnimationData AssetLoader<AnimationData>::LoadImpl(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

    if (!scene || !scene->mAnimations || scene->mNumAnimations == 0) {
        std::cerr << "ERROR: Failed to load animation from " << path << std::endl;
        if (!scene) {
            std::cerr << "  Assimp error: " << importer.GetErrorString() << std::endl;
        } else {
            std::cerr << "  No animations found in file" << std::endl;
        }
        return AnimationData{};
    }

    // Load the first animation
    const aiAnimation* anim = scene->mAnimations[0];

    AnimationData animData;
    animData.Name = anim->mName.C_Str();
    animData.TicksPerSecond = anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 30.0f;
    animData.Duration = anim->mDuration / animData.TicksPerSecond;

    ANIM_LOG << "ANIM_LOADER: Loading animation '" << animData.Name << "'" << std::endl;
    ANIM_LOG << "  Duration: " << animData.Duration << "s (" << anim->mDuration << " ticks @ "
              << animData.TicksPerSecond << " tps)" << std::endl;
    ANIM_LOG << "  Channels: " << anim->mNumChannels << std::endl;

    // Load all bone animation channels
    for (unsigned int i = 0; i < anim->mNumChannels; i++) {
        const aiNodeAnim* channel = anim->mChannels[i];

        BoneAnimationChannel boneChannel;
        boneChannel.BoneName = channel->mNodeName.C_Str();

        // Load position keys - import as-is from Blender (no coordinate transformation)
        for (unsigned int j = 0; j < channel->mNumPositionKeys; j++) {
            const aiVectorKey& key = channel->mPositionKeys[j];
            PositionKey posKey;
            posKey.Time = key.mTime / animData.TicksPerSecond;
            // Import as-is from Blender (no coordinate transformation)
            posKey.Value = glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z);
            boneChannel.PositionKeys.push_back(posKey);
        }

        // Load rotation keys - import as-is from Blender (no coordinate transformation)
        for (unsigned int j = 0; j < channel->mNumRotationKeys; j++) {
            const aiQuatKey& key = channel->mRotationKeys[j];
            RotationKey rotKey;
            rotKey.Time = key.mTime / animData.TicksPerSecond;
            // Import as-is from Blender (no coordinate transformation)
            rotKey.Value = glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z);
            boneChannel.RotationKeys.push_back(rotKey);
        }

        // Load scale keys - import as-is from Blender (no coordinate transformation)
        for (unsigned int j = 0; j < channel->mNumScalingKeys; j++) {
            const aiVectorKey& key = channel->mScalingKeys[j];
            ScaleKey scaleKey;
            scaleKey.Time = key.mTime / animData.TicksPerSecond;
            // Import as-is from Blender (no coordinate transformation)
            scaleKey.Value = glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z);
            boneChannel.ScaleKeys.push_back(scaleKey);
        }

        animData.Channels.push_back(boneChannel);
    }

    ANIM_LOG << "  Loaded " << animData.Channels.size() << " bone channels" << std::endl;

    // Print first few bone names for debugging
    ANIM_LOG << "  First bones: ";
    for (size_t i = 0; i < std::min(size_t(5), animData.Channels.size()); i++) {
        ANIM_LOG << animData.Channels[i].BoneName;
        if (i < std::min(size_t(5), animData.Channels.size()) - 1) ANIM_LOG << ", ";
    }
    ANIM_LOG << std::endl;

    return animData;
}
