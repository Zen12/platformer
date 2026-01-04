#pragma once

#include "shader.hpp"
#include "../texture/texture.hpp"
#include "../../asset/loaders/asset_loader.hpp"
#include "../../font/font.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


enum class BlendMode {
    None = 0,
    AlphaAdditive = 1,
    ColorAdditive = 2,
    StandardAlpha = 3,
};

class Material
{
private:
    std::weak_ptr<Shader> _shader{};
    std::weak_ptr<Font> _font{};
    std::vector<std::weak_ptr<Texture>> _textures{};
    bool _isFaceCulled{};
    bool _isDepthTestEnabled{};
    BlendMode _blendMode{};


public:
    Material() = delete;

    explicit Material(std::weak_ptr<Shader> shader)
        : _shader(std::move(shader))
    {
    }

    [[nodiscard]] int32_t GetLocation(const std::string &key) const noexcept;

    void Bind() const;

    void UseShader() const {
        _shader.lock()->Use();
    }

    void ClearSprites() noexcept {
        _textures.clear();
    }

    void AddSprite(const std::weak_ptr<Texture> &sprite) noexcept
    {
        _textures.push_back(sprite);
    }

    [[nodiscard]] std::weak_ptr<Font> GetFont() const noexcept {
        return _font;
    }

    void SetFont(const std::weak_ptr<Font> &font) noexcept
    {
        _font = font;
    }

    void SetBlendMode(const BlendMode &blendMode) noexcept {
        _blendMode = blendMode;
    }

    [[nodiscard]] BlendMode GetBlendMode() const noexcept {
        return _blendMode;
    }

    void SetCulling(const bool &culling) noexcept {
        _isFaceCulled = culling;
    }

    [[nodiscard]] bool GetCulling() const noexcept {
        return _isFaceCulled;
    }

    void SetDepthTest(const bool &depthTest) noexcept {
        _isDepthTestEnabled = depthTest;
    }

    [[nodiscard]] bool GetDepthTest() const noexcept {
        return _isDepthTestEnabled;
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

    void SetMat4Array(const std::string &name, const std::vector<glm::mat4> &matrices) const noexcept  {
        if (const auto shader = _shader.lock())
        {
            shader->Use();
            const auto location = shader->GetLocation(name);
            shader->SetMat4Array(location, matrices);
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

    void SetFloat(const std::string &name, const float& value) const {

        if (const auto shader = _shader.lock())
        {
            shader->Use();
            const auto location = shader->GetLocation(name);
            shader->SetFloat(location, value);
        }
    }

    void BindTextures() const {
        for (size_t i = 0; i < _textures.size(); i++) {
            if (const auto texture = _textures[i].lock()) {
                texture->Bind(i);
            }
        }
    }

    void ApplyRenderState() const {
        if (_isFaceCulled) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        } else {
            glDisable(GL_CULL_FACE);
        }

        if (_isDepthTestEnabled) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
        } else {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
        }

        switch (_blendMode) {
            case BlendMode::None:
                glDisable(GL_BLEND);
                break;
            case BlendMode::AlphaAdditive:
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case BlendMode::ColorAdditive:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
            case BlendMode::StandardAlpha:
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                break;
        }
    }
};