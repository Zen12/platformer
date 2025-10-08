#pragma once

#include <yaml-cpp/yaml.h>

#include "character_animation_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<AnimationTransition>
    {
        static bool decode(const Node &node, AnimationTransition &rhs)
        {
            if (node["from"]) {
                rhs.From = node["from"].as<std::string>();
            }
            if (node["to"]) {
                rhs.To = node["to"].as<std::string>();
            }
            if (node["duration"]) {
                rhs.Duration = node["duration"].as<float>();
            }
            return true;
        }
    };

    template <>
    struct convert<CharacterAnimationComponentSerialization>
    {
        static bool decode(const Node &node, CharacterAnimationComponentSerialization &rhs)
        {
            if (node["transitions"]) {
                rhs.Transitions = node["transitions"].as<std::vector<AnimationTransition>>();
            }
            return true;
        }
    };
}