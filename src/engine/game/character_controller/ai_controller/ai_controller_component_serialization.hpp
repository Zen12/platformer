#pragma once
#include "../../../entity/component_serialization.hpp"

struct AiControllerComponentSerialization final : public ComponentSerialization
{
    float Damage{10};

    std::string AiTargetTransformTag{};
    std::string PathFinderTag{};
    std::string GridTag{};
};