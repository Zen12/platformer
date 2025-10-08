#pragma once
#include "../../../entity/component_serialization.hpp"
#include <string>

struct ShootComponentSerialization final : public ComponentSerialization {
    std::string BoneName = "gun-tip";
    float RaycastDistance = 1000.0f;
};
