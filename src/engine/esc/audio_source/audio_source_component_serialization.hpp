#pragma once
#include <string>
#include "../../entity/component_serialization.hpp"

struct AudioSourceComponentSerialization final : public ComponentSerialization {
    std::string AudioClipGuid;
    float Volume = 1.0f;
    float Pitch = 1.0f;
    bool Loop = false;
    bool AutoPlay = false;
    bool Spatial = true;
    float MinDistance = 1.0f;
    float MaxDistance = 100.0f;
};
