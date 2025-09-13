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
            glEnable(GL_CULL_FACE); // spine runtime requires
        } else {
            glDisable(GL_CULL_FACE);
        }

        switch (_blendMode) {
            case BlendMode::None:
                glEnable(GL_BLEND);
                break;
            case BlendMode::AlphaAdditive:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                break;
            case BlendMode::ColorAdditive:
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
        }

        shader->Use();

        for (const auto &value : _sprites)
        {
            if (const auto sprite = value.lock())
            {
                sprite->Bind();
            }
        }
    }
}