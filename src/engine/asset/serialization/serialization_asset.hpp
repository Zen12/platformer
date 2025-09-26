#pragma once
#include <string>
#include <vector>

#include "serialization_component.hpp"


struct SceneAsset
{
    std::string Type;
    std::vector<EntitySerialization> Entities;
};

struct PrefabAsset
{
    EntitySerialization Obj;
};

struct MetaAsset
{
    std::string Guid{};
    std::string Extension{};
    std::string Type{};
    std::string Path{};
};

struct MaterialAsset
{
    std::string VertexShaderGuid{};
    std::string FragmentShaderGuid{};
    std::string Font{};
    std::string Image{};
    int BlendMode = 0;
    bool IsCulling = false;
};

struct ProjectAsset
{
    std::string Name{};
    std::vector<std::string> Scenes{};
    std::vector<int> Resolution{};
    float TargetFps{60.0f};
};

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




