#pragma once
#include <string>
#include <yaml-cpp/yaml.h>
#include <glm/vec3.hpp>


struct ComponentSerialization
{
    virtual ~ComponentSerialization() = default;
};

struct EntitySerialization
{
    std::string Tag;
    std::string Guid;
    std::vector<std::unique_ptr<ComponentSerialization>> Components;
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
    glm::vec3 scale{};
};

struct Rigidbody2dSerialization final : public ComponentSerialization
{
    bool isDynamic = false;
};


struct CameraComponentSerialization final : public ComponentSerialization
{
    float aspectPower{};
    bool isPerspective{};
};

struct ShaderComponentSerialization final : public ComponentSerialization
{
    std::string vertexSourceCode;
    std::string fragmentSourceCode;

    ShaderComponentSerialization(std::string vertex, std::string fragment)
        : vertexSourceCode(std::move(vertex)), fragmentSourceCode(std::move(fragment))
    {
    }
};
struct SpineRenderComponentSerialization final : public ComponentSerialization
{
    std::string SpineGuid;
};

struct SpriteRenderComponentSerialization final : public ComponentSerialization
{
    std::string MaterialGuid;
    std::string SpriteGuid;
};

struct MeshRendererComponentSerialization final : public ComponentSerialization
{
    std::string MaterialGuid;
};

struct Light2dComponentSerialization final : public ComponentSerialization
{
    std::string CenterTransform;
    glm::vec3 Offset;
};

struct SpriteComponentSerialization final : public ComponentSerialization
{
    std::string Path;
};

struct MaterialComponentSerialization final : public ComponentSerialization
{
    ShaderComponentSerialization Shader;

    explicit MaterialComponentSerialization(ShaderComponentSerialization shader)
        : Shader(std::move(shader))
    {
    }
};

struct UiTextComponentSerialization final : public ComponentSerialization {
    std::string MaterialGUID;
    std::string Text;
};

struct UiImageComponentSerialization final : public ComponentSerialization
{
    std::string MaterialGuid;
    std::string SpriteGuid;
};

struct RectTransformComponentSerialization final : public ComponentSerialization
{
    std::vector<RectLayoutSerialization> Layouts;
};

struct TransformComponentSerialization final : public ComponentSerialization
{
    glm::vec3 position{};
    glm::vec3 rotation{};
    glm::vec3 scale{};
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

};
