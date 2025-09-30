#pragma once
#include "../../entity/component_serialization.hpp"

struct AiControllerComponentSerialization final : public ComponentSerialization
{
    // movement
    float MaxMovementSpeed{1};
    float AccelerationSpeed{1};
    float DecelerationSpeed{1};

    // jump
    float JumpHeigh{1};
    float JumpDuration{1};
    float JumpDownMultiplier{1};
    float AirControl{0.1};

    float Damage{10};

    std::string AiTargetTransformTag{};
    std::string PathFinderTag{};
    std::string GridTag{};
};