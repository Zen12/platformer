#pragma once
#include <string>

struct SpineAsset
{
    std::string skeleton;
    std::string atlas;
    std::string image;
    std::string json;
    float spineScale;
    std::string moveAnimationName;
    std::string jumpAnimationName;
    std::string hitAnimationName;
    std::string idleAnimationName;
};

