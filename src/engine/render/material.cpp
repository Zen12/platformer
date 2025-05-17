#include "material.h"

int32_t Material::GetLocation(const std::string& key)
{
    auto it = _locationMap.find(key);
    if (it != _locationMap.end()) {
            return it->second;
    }

        int location = _shader->GetLocation(key);
        _locationMap[key] = location;
        return location;

}

void Material::Bind() const
{
        _shader->Use();
        
        for (const auto value : _sprites) {
            if (const auto sprite = value.lock())
            {
                sprite->Bind();
            }
        }
}