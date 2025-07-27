#pragma once
#include <string>
#include <vector>

#include "asset_deserializer.hpp"


struct Asset {
    std::string Name{};
};

struct SceneAsset : Asset
{
    std::string Type;
    std::vector<EntitySerialization> Entities;
};

template <>
struct YAML::convert<SceneAsset>
{
    static bool decode(const Node &node, SceneAsset &rhs)
    {
        rhs.Name = node["name"].as<std::string>();
        rhs.Type = node["type"].as<std::string>();
        rhs.Entities = node["entities"].as<std::vector<EntitySerialization>>();
        return true;
    }
};

struct MetaAsset : Asset
{
    std::string Guid{};
    std::string Extension{};
    std::string Type{};
    std::string Path{};
};

template <>
struct YAML::convert<MetaAsset>
{
    static bool decode(const Node &node, MetaAsset &rhs)
    {
        rhs.Name = node["name"].as<std::string>();
        rhs.Guid = node["guid"].as<std::string>();
        rhs.Extension = node["extension"].as<std::string>();
        rhs.Type = node["type"].as<std::string>();

        return true;
    }
};


struct MaterialAsset : Asset
{
    std::string VertexShaderGuid{};
    std::string FragmentShaderGuid{};
    std::string Font{};
};

template <>
struct YAML::convert<MaterialAsset>
{
    static bool decode(const Node &node, MaterialAsset &rhs)
    {
        rhs.Name = node["name"].as<std::string>();
        if (const auto shaderNode = node["shader"]) {
            rhs.VertexShaderGuid = shaderNode["vertex"].as<std::string>();
            rhs.FragmentShaderGuid = shaderNode["fragment"].as<std::string>();
        }

        if (node["font"] && node["font"].IsScalar()) {
            rhs.Font = node["font"].as<std::string>();
        }

        return true;
    }
};

struct ProjectAsset
{
    std::string Name{};
    std::vector<std::string> Scenes{};
    std::vector<int> Resolution{};
};

template <>
struct YAML::convert<ProjectAsset>
{
    static bool decode(const Node &node, ProjectAsset &rhs)
    {
        rhs.Name = node["name"].as<std::string>();
        rhs.Scenes = node["scenes"].as<std::vector<std::string>>();
        rhs.Resolution = node["resolution"].as<std::vector<int>>();
        return true;
    }
};

struct SpineAsset : Asset {
    std::string skeleton{};
    std::string atlas{};
    std::string image{};
    std::string json{};
};

template <>
struct YAML::convert<SpineAsset>
{
    static bool decode(const Node &node, SpineAsset &rhs)
    {
        rhs.Name = node["name"].as<std::string>();
        rhs.skeleton = node["skeleton"].as<std::string>();
        rhs.atlas = node["atlas"].as<std::string>();
        rhs.image = node["image"].as<std::string>();
        rhs.json = node["json"].as<std::string>();
        return true;
    }
};




