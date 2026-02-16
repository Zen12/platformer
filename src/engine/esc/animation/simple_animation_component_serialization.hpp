#pragma once
#include "entity/component_serialization.hpp"
#include "guid.hpp"

struct SimpleAnimationComponentSerialization final : public ComponentSerialization {
    Guid AnimationGuid;

    ~SimpleAnimationComponentSerialization() override = default;
};
