#pragma once
#include "animation_data.hpp"
#include "../../asset/loaders/asset_loader.hpp"
#include <iostream>

template <>
class AssetLoader<AnimationData> : public AssetLoaderBase<AnimationData, AssetLoader<AnimationData>> {
public:
    static void Init();
    static AnimationData LoadImpl(const std::string& path);
};
