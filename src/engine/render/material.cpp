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