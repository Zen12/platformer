#include "material.h"

int32_t Material::GetLocation(const std::string& key)
{
    auto it = _locationMap.find(key);
    if (it != _locationMap.end()) {
            return it->second;
    }

    if (const auto shader = _shader.lock())
    {
        int location = shader->GetLocation(key);
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
        
        for (const auto value : _sprites) {
            if (const auto sprite = value.lock())
            {
                sprite->Bind();
            }
        }
    }
}