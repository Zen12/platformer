#pragma once

#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
private:
    int32_t _shaderProgram;
    uint32_t _fragmentShader;
    uint32_t _vertexShader;
    std::unordered_map<std::string, int32_t> _locationMap;

public:
    Shader() = default;
    Shader(const std::string &vertexSource, const std::string &fragmentSource);
    ~Shader();

    void Use() const;
    int32_t GetShaderId() const
    {
        return _shaderProgram;
    }

    int32_t GetLocation(const std::string &name) const;

    void SetVec2(const int32_t &location, const float &x, const float &y) const;
    void SetVec3(const int32_t &location, const float &x, const float &y, const float &z) const;
    void SetMat4(const int32_t &location, const glm::mat4 &mat) const;
};
