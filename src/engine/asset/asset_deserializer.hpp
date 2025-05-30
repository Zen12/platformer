#include <string>
#include <yaml-cpp/yaml.h>
#include <vector>

struct ComponentData
{
    int x;
    int y;
};

struct Component
{
    std::string guid;
    ComponentData data;
};

struct Entity
{
    std::string guid;
    std::vector<Component> components;
};

struct HierarchyNode
{
    std::string guid;
    std::vector<std::string> children;
};

struct Scene
{
    std::string name;
    std::string type;
    std::vector<HierarchyNode> hierarchy;
    std::vector<Entity> entities;
};

struct CameraData
{
    float aspectPower;
    bool isPerspective;
};

/*

name: main_screen
type: scene
hierarchy:
  - guid: obj-id-1
    children []

  - guid: obj-id-3
    children:
        - obj-id-2

entities:
  - guid: obj-id-1
    components:
      - type: camera
        data: {aspectPower: 20.0, isPerspective: false}
      - type: tranform
        data: {position: {x: 20.0, y: 30.0, z: 40.0}, rotation: {x: 20.0, y: 30.0, z: 40.0}, scale: {x: 1.0, y: 1.0, z: 1.0}}
  - guid: obj-id-3
        components:
      - type: rect_transform
        data: {}
      - type: center_x_layout_rect
        data: {}
*/
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

// ---------- Data Structures ----------

struct Vec3
{
    float x, y, z;
};

struct Component
{
    virtual ~Component() = default;
    virtual std::string getType() const = 0;
};

struct CameraComponent : public Component
{
    float aspectPower;
    bool isPerspective;

    std::string getType() const override { return "camera"; }
};

struct TransformComponent : public Component
{
    Vec3 position;
    Vec3 rotation;
    Vec3 scale;

    std::string getType() const override { return "tranform"; }
};

struct Entity
{
    std::string guid;
    std::vector<std::unique_ptr<Component>> components;
};

struct HierarchyNode
{
    std::string guid;
    std::vector<std::string> children;
};

struct Scene
{
    std::string name;
    std::string type;
    std::vector<HierarchyNode> hierarchy;
    std::vector<Entity> entities;
};

// ---------- YAML Converters ----------

namespace YAML
{

    // Vec3
    template <>
    struct convert<Vec3>
    {
        static bool decode(const Node &node, Vec3 &rhs)
        {
            rhs.x = node["x"].as<float>();
            rhs.y = node["y"].as<float>();
            rhs.z = node["z"].as<float>();
            return true;
        }
    };

    // HierarchyNode
    template <>
    struct convert<HierarchyNode>
    {
        static bool decode(const Node &node, HierarchyNode &rhs)
        {
            rhs.guid = node["guid"].as<std::string>();
            rhs.children = node["children"].as<std::vector<std::string>>();
            return true;
        }
    };

} // namespace YAML

// ---------- Component Factory ----------

std::unique_ptr<Component> createComponentFromYAML(const YAML::Node &node)
{
    const std::string type = node["type"].as<std::string>();
    const YAML::Node &data = node["data"];

    if (type == "camera")
    {
        auto comp = std::make_unique<CameraComponent>();
        comp->aspectPower = data["aspectPower"].as<float>();
        comp->isPerspective = data["isPerspective"].as<bool>();
        return comp;
    }
    else if (type == "tranform")
    {
        auto comp = std::make_unique<TransformComponent>();
        comp->position = data["position"].as<Vec3>();
        comp->rotation = data["rotation"].as<Vec3>();
        comp->scale = data["scale"].as<Vec3>();
        return comp;
    }

    throw std::runtime_error("Unknown component type: " + type);
}

// ---------- YAML Converters Continued ----------

namespace YAML
{

    template <>
    struct convert<Entity>
    {
        static bool decode(const Node &node, Entity &rhs)
        {
            rhs.guid = node["guid"].as<std::string>();
            for (const auto &compNode : node["components"])
            {
                rhs.components.push_back(createComponentFromYAML(compNode));
            }
            return true;
        }
    };

    template <>
    struct convert<Scene>
    {
        static bool decode(const Node &node, Scene &rhs)
        {
            rhs.name = node["name"].as<std::string>();
            rhs.type = node["type"].as<std::string>();
            rhs.hierarchy = node["hierarchy"].as<std::vector<HierarchyNode>>();
            rhs.entities = node["entities"].as<std::vector<Entity>>();
            return true;
        }
    };

} // namespace YAML

// ---------- Main ----------

int main()
{
    try
    {
        YAML::Node root = YAML::LoadFile("scene.yaml");
        Scene scene = root.as<Scene>();

        std::cout << "Scene: " << scene.name << " (" << scene.type << ")\n";
        for (const auto &entity : scene.entities)
        {
            std::cout << "- Entity: " << entity.guid << "\n";
            for (const auto &comp : entity.components)
            {
                std::cout << "  * Component: " << comp->getType() << "\n";

                if (comp->getType() == "camera")
                {
                    auto *cam = dynamic_cast<CameraComponent *>(comp.get());
                    std::cout << "      aspectPower: " << cam->aspectPower
                              << ", isPerspective: " << cam->isPerspective << "\n";
                }
                else if (comp->getType() == "tranform")
                {
                    auto *tf = dynamic_cast<TransformComponent *>(comp.get());
                    std::cout << "      position: (" << tf->position.x << ", "
                              << tf->position.y << ", " << tf->position.z << ")\n";
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
