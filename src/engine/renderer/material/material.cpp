#include "material.hpp"

int32_t Material::GetLocation(const std::string &key) const noexcept
{
    if (const auto shader = _shader.lock())
    {
        const auto location = shader->GetLocation(key);
        return location;
    }

    return -1;
}

void Material::Bind() const
{
    if (const auto shader = _shader.lock())
    {
        if (_isFaceCulled) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        } else {
            glDisable(GL_CULL_FACE);
        }

        if (_isDepthTestEnabled) {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);  // Enable depth writes for 3D
        } else {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);  // Disable depth writes for UI
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

        shader->Use();

        for (size_t i = 0; i < _textures.size(); i++) {
            if (const auto texture = _textures[i].lock())
            {
                texture->Bind(i);
            }
        }
    }
}