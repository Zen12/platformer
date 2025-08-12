#pragma once

#include <string>
#include <yaml-cpp/yaml.h>
#include <utility>
#include <vector>
#include <iostream>
#include <memory>
#include "../components/entity.hpp"

struct Vec3
{
    float x, y, z;
};

struct RectLayoutSerialization
{
    const std::string Type;
    const float Value = 0;
    const float X = 0;
    const float Y = 0;

    RectLayoutSerialization(std::string type, const float value, const float &x, const float &y)
    : Type(std::move(type)), Value(value), X(x), Y(y)
    {
    }
};

struct Box2dColliderSerialization final : public ComponentSerialization
{
    Vec3 scale{};

    [[nodiscard]] std::string getType() const override { return "box_collider"; }
};

struct Rigidbody2dSerialization final : public ComponentSerialization
{
    bool isDynamic = false;

    [[nodiscard]] std::string getType() const override { return "rigidbody2d"; }
};


struct CameraComponentSerialization final : public ComponentSerialization
{
    float aspectPower{};
    bool isPerspective{};

    [[nodiscard]] std::string getType() const override { return "camera"; }
};

struct ShaderComponentSerialization final : public ComponentSerialization
{
    std::string vertexSourceCode;
    std::string fragmentSourceCode;

    ShaderComponentSerialization(std::string vertex, std::string fragment)
        : vertexSourceCode(std::move(vertex)), fragmentSourceCode(std::move(fragment))
    {
    }

    [[nodiscard]] std::string getType() const override
    {
        return "shader";
    }
};
struct SpineRenderComponentSerialization final : public ComponentSerialization
{
    const std::string SpineGuid;

    explicit SpineRenderComponentSerialization(std::string spineGuid)
        : SpineGuid(std::move(spineGuid))
    {
    }

    [[nodiscard]] std::string getType() const override
    {
        return "spine_renderer";
    }
};

struct SpriteRenderComponentSerialization final : public ComponentSerialization
{
    const std::string MaterialGuid;
    const std::string SpriteGuid;

    explicit SpriteRenderComponentSerialization(std::string materialGuid, std::string spriteGuid)
        : MaterialGuid(std::move(materialGuid)), SpriteGuid(std::move(spriteGuid))
    {
    }

    [[nodiscard]] std::string getType() const override
    {
        return "sprite_renderer";
    }
};

struct MeshRendererComponentSerialization final : public ComponentSerialization
{
    const std::string MaterialGuid;

    explicit MeshRendererComponentSerialization(std::string materialGuid)
        : MaterialGuid(std::move(materialGuid))
    {
    }

    [[nodiscard]] std::string getType() const override
    {
        return "mesh_renderer";
    }
};

struct Light2dComponentSerialization final : public ComponentSerialization
{
    const std::string CenterTransform;
    const float OffsetX;
    const float OffsetY;

    Light2dComponentSerialization(std::string centerTransform, const float offsetX, const float offsetY)
        : CenterTransform(std::move(centerTransform)), OffsetX(offsetX), OffsetY(offsetY)
    {
    }

    [[nodiscard]] std::string getType() const override
    {
        return "light_2d";
    }
};

struct SpriteComponentSerialization final : public ComponentSerialization
{
    std::string Path;
    explicit SpriteComponentSerialization(std::string path)
        : Path(std::move(path))
    {
    }

    [[nodiscard]] std::string getType() const override
    {
        return "sprite";
    }
};

struct MaterialComponentSerialization final : public ComponentSerialization
{
    ShaderComponentSerialization Shader;

    explicit MaterialComponentSerialization(ShaderComponentSerialization shader)
        : Shader(std::move(shader))
    {
    }

    [[nodiscard]] std::string getType() const override
    {
        return "material";
    }
};

struct UiTextComponentSerialization final : public ComponentSerialization {
    std::string MaterialGUID;
    std::string Text;

    [[nodiscard]] std::string getType() const override
    {
        return "ui_text";
    }
};

struct UiImageComponentSerialization final : public ComponentSerialization
{
    std::string MaterialGUID;
    std::string SpriteGUID;

    [[nodiscard]] std::string getType() const override
    {
        return "ui_image";
    }
};

struct RectTransformComponentSerialization final : public ComponentSerialization
{
    std::vector<RectLayoutSerialization> Layouts;

    [[nodiscard]] std::string getType() const override
    {
        return "rect_transform";
    }
};

struct TransformComponentSerialization final : public ComponentSerialization
{
    Vec3 position{};
    Vec3 rotation{};
    Vec3 scale{};

    [[nodiscard]] std::string getType() const override { return "transform"; }
};

struct CharacterControllerComponentSerialization final : public ComponentSerialization
{
    // movement
    float MaxMovementSpeed{1};
    float AccelerationSpeed{1};
    float DecelerationSpeed{1};

    // jump
    float JumpHeigh{1};
    float JumpDuration{1};
    float JumpDownMultiplier{1};
    float AirControl{0.1};

    [[nodiscard]] std::string getType() const override { return "character_controller"; }
};

struct EntitySerialization
{
    std::string Tag;
    std::string Guid;
    std::vector<std::unique_ptr<ComponentSerialization>> Components;
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

} // namespace YAML

// ---------- Component Factory ----------

inline std::unique_ptr<UiTextComponentSerialization> createUiText(const YAML::Node &map) {
    auto comp = std::make_unique<UiTextComponentSerialization>();
    comp->MaterialGUID = map["material"].as<std::string>();
    comp->Text = map["text"].as<std::string>();
    return comp;
}


inline std::unique_ptr<SpineRenderComponentSerialization> createSpineRenderer(const YAML::Node &map)
{
    return std::make_unique<SpineRenderComponentSerialization>(
         map["spine_data"].as<std::string>());
}

inline std::unique_ptr<MeshRendererComponentSerialization> createMeshRenderer(const YAML::Node &map)
{
    return std::make_unique<MeshRendererComponentSerialization>(
        map["material"].as<std::string>());
}

inline std::unique_ptr<Light2dComponentSerialization> createLight2dRenderer(const YAML::Node &map)
{
    return std::make_unique<Light2dComponentSerialization>(
        map["center"].as<std::string>(),
        map["offset_x"].as<float>(),
        map["offset_y"].as<float>());
}

inline std::unique_ptr<SpriteRenderComponentSerialization> createSpriteRenderer(const YAML::Node &map)
{
    return std::make_unique<SpriteRenderComponentSerialization>(
        map["material"].as<std::string>(), map["image"].as<std::string>());
}

inline std::unique_ptr<UiImageComponentSerialization> createUiImage(const YAML::Node &map)
{
    auto comp = std::make_unique<UiImageComponentSerialization>();
    comp->MaterialGUID = map["material"].as<std::string>();
    comp->SpriteGUID = map["image"].as<std::string>();
    return comp;
}

inline std::unique_ptr<RectTransformComponentSerialization> createRectTransform(const YAML::Node &map)
{
    auto comp = std::make_unique<RectTransformComponentSerialization>();

    if (!map.IsMap())
    {
        std::cerr << "Expected a map node.\n";
        return comp;
    }

    for (const auto &kv : map)
    {
        const auto key = kv.first.as<std::string>();
        YAML::Node list = kv.second;

        auto x = 0.0f;
        auto y = 0.0f;
        auto value =  0.0f;

        for (const auto &item : list) {
            x = item["x"] ? item["x"].as<float>() : x;
            y = item["y"] ? item["y"].as<float>() : y;
            value = item["value"] ? item["value"].as<float>() : value;
        }


        comp->Layouts.emplace_back(key, value, x, y);
    }

    return comp;
}

inline std::unique_ptr<RectTransformComponentSerialization> createMaterial(const YAML::Node &map)
{
    auto comp = std::make_unique<RectTransformComponentSerialization>();

    if (!map.IsMap())
    {
        std::cerr << "Expected a map node.\n";
        return comp;
    }

    for (const auto &kv : map)
    {
        const auto key = kv.first.as<std::string>();

        if (const YAML::Node list = kv.second; list.IsSequence() && list.size() == 1 && list[0]["value"])
        {
            auto val = list[0]["value"].as<float>();
            comp->Layouts.emplace_back(key, val, 0.0f, 0.0f);
        }
        else
        {
            std::cerr << "Node is not a sequence with one element having 'value' key\n";
        }
    }

    return comp;
}

inline std::unique_ptr<ComponentSerialization> createTransform(const YAML::Node &map)
{
    auto comp = std::make_unique<TransformComponentSerialization>();
    comp->position = map["position"].as<Vec3>();
    comp->rotation = map["rotation"].as<Vec3>();
    comp->scale = map["scale"].as<Vec3>();
    return comp;
}

inline std::unique_ptr<ComponentSerialization> createCamera(const YAML::Node &map)
{
    auto comp = std::make_unique<CameraComponentSerialization>();
    comp->aspectPower = map["aspectPower"].as<float>();
    comp->isPerspective = map["isPerspective"].as<bool>();
    return comp;
}


inline std::unique_ptr<Rigidbody2dSerialization> createRigidBody(const YAML::Node &map)
{
    auto comp = std::make_unique<Rigidbody2dSerialization>();
    comp->isDynamic = map["isDynamic"].as<bool>();
    return comp;
}

inline std::unique_ptr<Box2dColliderSerialization> createBoxCollider(const YAML::Node &map)
{
    auto comp = std::make_unique<Box2dColliderSerialization>();
    comp->scale = map["size"].as<Vec3>();
    return comp;
}

inline std::unique_ptr<CharacterControllerComponentSerialization> createCharacterController(const YAML::Node & map) {
    auto comp = std::make_unique<CharacterControllerComponentSerialization>();
    comp->MaxMovementSpeed = map["max_movement_speed"].as<float>();
    comp->AccelerationSpeed = map["acceleration_speed"].as<float>();
    comp->DecelerationSpeed = map["deceleration_speed"].as<float>();
    comp->JumpHeigh = map["jump_heigh"].as<float>();
    comp->JumpDuration = map["jump_duration"].as<float>();
    comp->JumpDownMultiplier = map["jump_down_multiplier"].as<float>();
    comp->AirControl = map["air_control"].as<float>();
    return comp;
}

inline std::unique_ptr<ComponentSerialization> createComponentFromYAML(const YAML::Node &node)
{
    const auto nodeMap = sequenceToMap(node);
    const auto type = nodeMap["type"].as<std::string>();
    const YAML::Node &data = nodeMap["data"];
    const auto map = sequenceToMap(data);

    if (type == "camera")
    {
        return createCamera(map);
    }
    else if (type == "transform")
    {
        return createTransform(map);
    }
    else if (type == "rect_transform") {
        return createRectTransform(map);
    } else if (type == "ui_image") {
        return createUiImage(map);
    } if (type == "ui_text") {
        return createUiText(map);
    } else if (type == "sprite_renderer") {
        return createSpriteRenderer(map);
    } else if (type == "box_collider") {
        return createBoxCollider(map);
    } else if (type == "rigidbody2d") {
        return createRigidBody(map);
    } else if (type == "light_2d") {
        return createLight2dRenderer(map);
    } if (type == "mesh_renderer") {
        return createMeshRenderer(map);
    } if (type == "spine_renderer") {
        return createSpineRenderer(map);
    } if (type == "character_controller") {
        return createCharacterController(map);
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
                const auto nodeMap = sequenceToMap(objNode);

                rhs.Guid = nodeMap["guid"].as<std::string>();
                if (nodeMap["tag"]) {
                    rhs.Tag = nodeMap["tag"].as<std::string>();
                }

                for (const auto &compNode : nodeMap["components"])
                {
                    if (const auto componentNode = compNode["component"])
                    {
                        rhs.Components.push_back(createComponentFromYAML(componentNode));
                    }
                }
            }
            return true;
        }
    };
} // namespace YAML
