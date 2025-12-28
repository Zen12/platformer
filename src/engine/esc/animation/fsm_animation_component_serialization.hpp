#pragma once
#include "../../entity/component_serialization.hpp"
#include <string>

struct FsmAnimationComponentSerialization final : public ComponentSerialization {
    std::string FsmGuid;
    bool Loop = true;
};
