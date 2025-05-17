#pragma once

#include <string>
#include <iostream>


class Shader
{
private:
    uint32_t _shaderProgram;
    uint32_t _fragmentShader;
    uint32_t _vertexShader;
    std::unordered_map<std::string, int32_t> _locationMap;

public:
    Shader() = default;
    Shader(const std::string& vertexSource, const std::string& fragmentSource);
    ~Shader();

    void Use() const;
    uint32_t GetShaderId() const
    {
        return _shaderProgram;
    }

    const int32_t GetLocation(const std::string& name) const;

    const void SetVec3(const int32_t& location, const float& x, const float& y, const float& z) const;
};

