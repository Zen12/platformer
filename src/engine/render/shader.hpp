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
    Shader(const std::string &vertexSource, const std::string &fragmentSource);
    ~Shader();

    Shader(const Shader &other) = default;
    Shader &operator=(const Shader &other) = default;

    Shader(Shader &&other) noexcept = delete;
    Shader &operator=(Shader &&other) noexcept = delete;

    void Use() const noexcept;
    [[nodiscard]] int32_t GetShaderId() const noexcept;
    [[nodiscard]] int32_t GetLocation(const std::string &name) const noexcept;

    void SetVec4(const int32_t &location, const float &x, const float &y, const float &z,
                 const float &w) const noexcept;

    void SetVec2(const int32_t &location, const float &x, const float &y) const noexcept;
    void SetVec3(const int32_t &location, const float &x, const float &y, const float &z) const noexcept;
    void SetMat4(const int32_t &location, const glm::mat4 &mat) const noexcept;

    void SetInt(const int32_t &location, const int &x) const noexcept;
    void SetFloat(const int32_t &location, const float &x) const noexcept;
};
