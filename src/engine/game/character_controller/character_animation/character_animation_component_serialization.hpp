#pragma once

#include "../../../entity/component_serialization.hpp"
#include <string>
#include <vector>

struct AnimationTransition {
    std::string From;
    std::string To;
    float Duration = 0.3f;
};

struct CharacterAnimationComponentSerialization : public ComponentSerialization {
    std::vector<AnimationTransition> Transitions;
};