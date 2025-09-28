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












struct ShaderComponentSerialization final : public ComponentSerialization
{
    std::string vertexSourceCode;
    std::string fragmentSourceCode;

    ShaderComponentSerialization(std::string vertex, std::string fragment)
        : vertexSourceCode(std::move(vertex)), fragmentSourceCode(std::move(fragment))
    {
    }
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















