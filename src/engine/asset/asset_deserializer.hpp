#pragma once

#include <string>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <iostream>
#include <memory>
#include <string>

// ---------- Data Structures ----------

struct Vec3
{
    float x, y, z;
};

struct ComponentSerialization
{
    virtual ~ComponentSerialization() = default;
    virtual std::string getType() const = 0;
};

struct CameraComponentSerialization : public ComponentSerialization
{
    float aspectPower;
    bool isPerspective;

    std::string getType() const override { return "camera"; }
};

struct TransformComponentSerialization : public ComponentSerialization
{
    Vec3 position;
    Vec3 rotation;
    Vec3 scale;

    std::string getType() const override { return "tranform"; }
};

struct EntitySerialization
{
    std::string guid;
    std::vector<std::unique_ptr<ComponentSerialization>> components;
};

struct HierarchyNodeSerialization
{
    std::string guid;
    std::vector<std::string> children;
};

struct SceneSerialization
{
    std::string name;
    std::string type;
    std::vector<HierarchyNodeSerialization> hierarchy;
    std::vector<EntitySerialization> entities;
};

// Helper to extract a map from a YAML node that is a sequence of single-key maps
inline YAML::Node sequenceToMap(const YAML::Node &seq)
{
    YAML::Node result(YAML::NodeType::Map);
    for (const auto &item : seq)
    {
        if (item.IsMap() && item.size() == 1)
        {
            for (const auto &kv : item)
            {
                result[kv.first] = kv.second;
            }
        }
    }
    return result;
}

// ---------- YAML Converters ----------

namespace YAML
{

    // Vec3
    template <>
    struct convert<Vec3>
    {
        static bool decode(const Node &node, Vec3 &rhs)
        {
            const auto map = sequenceToMap(node);
            rhs.x = map["x"].as<float>();
            rhs.y = map["y"].as<float>();
            rhs.z = map["z"].as<float>();
            return true;
        }
    };

    // HierarchyNode
    template <>
    struct convert<HierarchyNodeSerialization>
    {
        static bool decode(const Node &node, HierarchyNodeSerialization &rhs)
        {
            if (const auto objNode = node["obj"])
            {
                const auto sequance = sequenceToMap(objNode);
                rhs.guid = sequance["guid"].as<std::string>();

                for (const auto &child_guid_node : sequance["children"])
                    rhs.children.push_back(child_guid_node.as<std::string>());
                return true;
            }
            return false;
        }
    };

} // namespace YAML

// ---------- Component Factory ----------

inline std::unique_ptr<ComponentSerialization> createComponentFromYAML(const YAML::Node &node)
{
    const auto nodeMap = sequenceToMap(node);
    const std::string type = nodeMap["type"].as<std::string>();
    const YAML::Node &data = nodeMap["data"];
    const auto map = sequenceToMap(data);

    if (type == "camera")
    {
        auto comp = std::make_unique<CameraComponentSerialization>();
        comp->aspectPower = map["aspectPower"].as<float>();
        comp->isPerspective = map["isPerspective"].as<bool>();
        return comp;
    }
    else if (type == "transform")
    {
        auto comp = std::make_unique<TransformComponentSerialization>();
        comp->position = map["position"].as<Vec3>();
        comp->rotation = map["rotation"].as<Vec3>();
        comp->scale = map["scale"].as<Vec3>();
        return comp;
    }
    throw std::runtime_error("Unknown component type: " + type);
}

// ---------- YAML Converters Continued ----------

namespace YAML
{

    template <>
    struct convert<EntitySerialization>
    {
        static bool decode(const Node &node, EntitySerialization &rhs)
        {
            if (const auto objNode = node["obj"])
            {
                const auto sequanceNode = sequenceToMap(objNode);

                rhs.guid = sequanceNode["guid"].as<std::string>();

                for (const auto &compNode : sequanceNode["components"])
                {
                    if (const auto componentNode = compNode["component"])
                    {
                        rhs.components.push_back(createComponentFromYAML(componentNode));
                    }
                }
            }
            return true;
        }
    };

    template <>
    struct convert<SceneSerialization>
    {
        static bool decode(const Node &node, SceneSerialization &rhs)
        {
            rhs.name = node["name"].as<std::string>();
            rhs.type = node["type"].as<std::string>();
            rhs.hierarchy = node["hierarchy"].as<std::vector<HierarchyNodeSerialization>>();
            rhs.entities = node["entities"].as<std::vector<EntitySerialization>>();
            return true;
        }
    };

} // namespace YAML
