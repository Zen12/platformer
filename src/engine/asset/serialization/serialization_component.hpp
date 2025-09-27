#pragma once
#include <string>
#include <yaml-cpp/yaml.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


struct ComponentSerialization
{
    virtual ~ComponentSerialization() = default;
};

struct EntitySerialization
{
    std::string Creator{};
    std::string Tag;
    std::string Guid;
    int Layer;
    std::vector<std::shared_ptr<ComponentSerialization>> Components;
};


struct Box2dColliderSerialization final : public ComponentSerialization
{
    glm::vec3 scale{};
    glm::vec3 translate{};
};



struct ParticleEmitterSerialization final : public ComponentSerialization {
    float duration;
    float startScale;
    float endScale;
    size_t count;
    glm::vec3 startVelocity;
    glm::vec3 endVelocity;
    glm::vec4 startColor;
    glm::vec4 endColor;
    std::string materialGuid;
};

struct SpineColliderSerialization final : public ComponentSerialization {

};



struct Rigidbody2dSerialization final : public ComponentSerialization
{
    bool isDynamic = false;
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
    glm::vec3 Color;
    glm::vec3 Offset;

    float Radius = 20;
    int Segments = 500;

    float MaxAngle = 360;
    float StartAngle = 0;
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



struct TransformComponentSerialization final : public ComponentSerialization
{
    glm::vec3 position{};
    glm::vec3 rotation{};
    glm::vec3 scale{};
};





struct RectTransformFollowerSerialization final : public ComponentSerialization {
    bool UseCreator;
    std::string Target;
    glm::vec2 Offset{};
};







