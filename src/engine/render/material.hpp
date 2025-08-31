#pragma once

#include "shader.hpp"
#include "sprite.hpp"
#include "../asset/loaders/asset_loader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class Material
{
private:
    std::weak_ptr<Shader> _shader{};
    std::weak_ptr<Font> _font{};
    std::vector<std::weak_ptr<Sprite>> _sprites{};


public:
    Material() = delete;

    explicit Material(std::weak_ptr<Shader> shader)
        : _shader(std::move(shader))
    {
    }

    int32_t GetLocation(const std::string &key) const noexcept;
    void Bind() const;

    void AddSprite(const std::weak_ptr<Sprite> &sprite) noexcept
    {
        _sprites.push_back(sprite);
    }

    [[nodiscard]] std::weak_ptr<Font> GetFont() const noexcept {
        return _font;
    }

    void SetFont(const std::weak_ptr<Font> &font) noexcept
    {
        _font = font;
    }

    [[nodiscard]] int32_t GetShaderId() const noexcept
    {
        if (const auto shader = _shader.lock())
        {
            return shader->GetShaderId();
        }
        return -1;
    }

    void SetMat4(const std::string &name, const glm::mat4 &mat) const noexcept  {
        if (const auto shader = _shader.lock())
        {
            shader->Use();
            const auto location = shader->GetLocation(name);
            shader->SetMat4(location, mat);
        }
    }

    void SetVec4(const std::string &name, const glm::vec4 &value) const noexcept {
        if (const auto shader = _shader.lock())
        {
            shader->Use();
            const auto location = shader->GetLocation(name);
            shader->SetVec4(location, value.x, value.y, value.z, value.w);
        }
    }

    void SetVec3(const std::string &name, const float x, const float y, const float z) const noexcept {
        if (const auto shader = _shader.lock())
        {
            shader->Use();
            const auto location = shader->GetLocation(name);
            shader->SetVec3(location, x, y, z);
        }
    }

    void SetVec2(const std::string &name, const float x, const float y) const noexcept {
        if (const auto shader = _shader.lock())
        {
            shader->Use();
            const auto location = shader->GetLocation(name);
            shader->SetVec2(location, x, y);
        }
    }

    void SetInt(const std::string &name, const int& value) const {

        if (const auto shader = _shader.lock())
        {
            shader->Use();
            const auto location = shader->GetLocation(name);
            shader->SetInt(location, value);
        }
    }
};