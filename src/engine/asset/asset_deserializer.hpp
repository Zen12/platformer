#pragma once

#include <string>
#include <yaml-cpp/yaml.h>
#include <utility>
#include <vector>
#include <iostream>
#include <memory>

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

    RectLayoutSerialization(std::string type, const float &value)
        : Type(std::move(type)), Value(value)
    {
    }

    RectLayoutSerialization(std::string type, const float &x, const float &y)
    : Type(std::move(type)), X(x), Y(y)
    {
    }
};

struct ComponentSerialization
{
    virtual ~ComponentSerialization() = default;
    [[nodiscard]] virtual std::string getType() const = 0;
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

struct SpriteRenderComponentSerialization final : public ComponentSerialization
{
    std::string MaterialGuid;
    std::string SpriteGuid;

    explicit SpriteRenderComponentSerialization(std::string materialGuid, std::string spriteGuid)
        : MaterialGuid(std::move(materialGuid)), SpriteGuid(std::move(spriteGuid))
    {
    }

    [[nodiscard]] std::string getType() const override
    {
        return "sprite_renderer";
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

struct EntitySerialization
{
    std::string guid;
    std::vector<std::unique_ptr<ComponentSerialization>> components;
};

struct SceneSerialization
{
    std::string name;
    std::string type;
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

} // namespace YAML

// ---------- Component Factory ----------

inline std::unique_ptr<UiTextComponentSerialization> createUiText(const YAML::Node &map) {
    auto comp = std::make_unique<UiTextComponentSerialization>();
    comp->MaterialGUID = map["material"].as<std::string>();
    comp->Text = map["text"].as<std::string>();
    return comp;
}

inline std::unique_ptr<SpriteRenderComponentSerialization> createSpriteRenderer(const YAML::Node &map)
{
    auto comp = std::make_unique<SpriteRenderComponentSerialization>(
        map["material"].as<std::string>(), map["image"].as<std::string>());
    return comp;
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

        if (list.IsSequence() && list.size() == 1 && list[0]["value"])
        {
            const auto val = list[0]["value"].as<float>();
            comp->Layouts.emplace_back(key, val);
        }

        if (list.IsSequence() && list.size() == 2 && list[0]["x"] && list[1]["y"])
        {
            const auto x = list[0]["x"].as<float>();
            const auto y = list[1]["y"].as<float>();

            comp->Layouts.emplace_back(key, x, y);
        }
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
            comp->Layouts.emplace_back(key, val);
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
    else if (type == "rect_transform")
    {
        return createRectTransform(map);
    }
    else if (type == "ui_image")
    {
        return createUiImage(map);
    } if (type == "ui_text") {
        return createUiText(map);
    } else if (type == "sprite_renderer") {
        return createSpriteRenderer(map);
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
            rhs.entities = node["entities"].as<std::vector<EntitySerialization>>();
            return true;
        }
    };

} // namespace YAML
