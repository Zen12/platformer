#pragma once
#include "../../entity/component_serialization.hpp"
#include "../../system/guid.hpp"

struct SimpleAnimationComponentSerialization final : public ComponentSerialization {
    Guid AnimationGuid;

    ~SimpleAnimationComponentSerialization() override = default;
};
