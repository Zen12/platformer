#include "animation_asset_loader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

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

    std::cout << "ANIM_LOADER: Loading animation '" << animData.Name << "'" << std::endl;
    std::cout << "  Duration: " << animData.Duration << "s (" << anim->mDuration << " ticks @ "
              << animData.TicksPerSecond << " tps)" << std::endl;
    std::cout << "  Channels: " << anim->mNumChannels << std::endl;

    // Load all bone animation channels
    for (unsigned int i = 0; i < anim->mNumChannels; i++) {
        const aiNodeAnim* channel = anim->mChannels[i];

        BoneAnimationChannel boneChannel;
        boneChannel.BoneName = channel->mNodeName.C_Str();

        // Load position keys with coordinate conversion
        // Blender (Y-up): X=right, Y=up, Z=back
        // Engine (Z-up):  X=right, Y=forward, Z=up
        // Transformation: Engine(x,y,z) = Blender(x,z,y)
        for (unsigned int j = 0; j < channel->mNumPositionKeys; j++) {
            const aiVectorKey& key = channel->mPositionKeys[j];
            PositionKey posKey;
            posKey.Time = key.mTime / animData.TicksPerSecond;
            // Convert from Blender to Engine coordinates: swap Y and Z
            posKey.Value = glm::vec3(key.mValue.x, key.mValue.z, key.mValue.y);
            boneChannel.PositionKeys.push_back(posKey);
        }

        // Load rotation keys with coordinate conversion
        // Quaternion components need to be swapped to match coordinate system
        for (unsigned int j = 0; j < channel->mNumRotationKeys; j++) {
            const aiQuatKey& key = channel->mRotationKeys[j];
            RotationKey rotKey;
            rotKey.Time = key.mTime / animData.TicksPerSecond;
            // Convert quaternion from Blender to Engine coordinates: swap Y and Z components
            rotKey.Value = glm::quat(key.mValue.w, key.mValue.x, key.mValue.z, key.mValue.y);
            boneChannel.RotationKeys.push_back(rotKey);
        }

        // Load scale keys with coordinate conversion
        // Scale axes also need to be swapped
        for (unsigned int j = 0; j < channel->mNumScalingKeys; j++) {
            const aiVectorKey& key = channel->mScalingKeys[j];
            ScaleKey scaleKey;
            scaleKey.Time = key.mTime / animData.TicksPerSecond;
            // Convert from Blender to Engine coordinates: swap Y and Z
            scaleKey.Value = glm::vec3(key.mValue.x, key.mValue.z, key.mValue.y);
            boneChannel.ScaleKeys.push_back(scaleKey);
        }

        animData.Channels.push_back(boneChannel);
    }

    std::cout << "  Loaded " << animData.Channels.size() << " bone channels" << std::endl;

    // Print first few bone names for debugging
    std::cout << "  First bones: ";
    for (size_t i = 0; i < std::min(size_t(5), animData.Channels.size()); i++) {
        std::cout << animData.Channels[i].BoneName;
        if (i < std::min(size_t(5), animData.Channels.size()) - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    return animData;
}
