#pragma once
#include <string>
#include <vector>

#include "serialization_component.hpp"


struct Asset {
    std::string Name{};
};

struct SceneAsset : Asset
{
    std::string Type;
    std::vector<EntitySerialization> Entities;
};

struct MetaAsset : Asset
{
    std::string Guid{};
    std::string Extension{};
    std::string Type{};
    std::string Path{};
};

struct MaterialAsset : Asset
{
    std::string VertexShaderGuid{};
    std::string FragmentShaderGuid{};
    std::string Font{};
};

struct ProjectAsset
{
    std::string Name{};
    std::vector<std::string> Scenes{};
    std::vector<int> Resolution{};
};

struct SpineAsset : Asset {
    std::string skeleton;
    std::string atlas;
    std::string image;
    std::string json;
    float spineScale;
};




