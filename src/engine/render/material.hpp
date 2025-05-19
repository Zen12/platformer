#pragma once

#include "shader.hpp"
#include "sprite.hpp"
#include "../asset/asset_loader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class Material
{
private:
    std::weak_ptr<Shader> _shader;
    std::weak_ptr<Font> _font;
    std::unordered_map<std::string, int32_t> _locationMap;
    std::vector<std::weak_ptr<Sprite>> _sprites;

public:
    Material() = delete;

    Material(const std::weak_ptr<Shader> &shader)
        : _shader(shader)
    {
    }

    int32_t GetLocation(const std::string &key);
    void Bind() const;

    void AddSprite(const std::weak_ptr<Sprite> &sprite)
    {
        _sprites.push_back(sprite);
    }

    const std::weak_ptr<Font> GetFont() const
    {
        return _font;
    }

    void SetFont(const std::weak_ptr<Font> font)
    {
        _font = font;
    }

    int32_t GetShaderId() const
    {
        if (const auto shader = _shader.lock())
        {
            return (int32_t)shader->GetShaderId();
        }
        return -1;
    }

    void SetMat4(const std::string &name, glm::mat4 mat)
    {
        if (const auto shader = _shader.lock())
        {
            shader->Use();
            const auto location = shader->GetLocation(name);
            shader->SetMat4(location, mat);
        }
    }

    void SetVec3(const std::string &name, float x, float y, float z)
    {
        if (const auto shader = _shader.lock())
        {
            shader->Use();
            const auto location = shader->GetLocation(name);
            shader->SetVec3(location, x, y, z);
        }
    }

    void SetVec2(const std::string &name, float x, float y)
    {
        if (const auto shader = _shader.lock())
        {
            shader->Use();
            const auto location = shader->GetLocation(name);
            shader->SetVec2(location, x, y);
        }
    }
};