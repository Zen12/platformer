#pragma once
#include <string>
#include <yaml-cpp/yaml.h>


struct ComponentSerialization
{
    virtual ~ComponentSerialization() = default;
    [[nodiscard]] virtual std::string getType() const = 0;
};


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