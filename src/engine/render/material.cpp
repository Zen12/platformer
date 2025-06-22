#include "material.hpp"

int32_t Material::GetLocation(const std::string &key)  noexcept
{
    if (const auto it = _locationMap.find(key); it != _locationMap.end())
    {
        return it->second;
    }

    if (const auto shader = _shader.lock())
    {
        const auto location = shader->GetLocation(key);
        _locationMap[key] = location;
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