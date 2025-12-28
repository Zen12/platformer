#pragma once
#include "../../entity/component_serialization.hpp"
#include <string>

struct SimpleAnimationComponentSerialization final : public ComponentSerialization {
    std::string AnimationGuid;

    ~SimpleAnimationComponentSerialization() override = default;
};
