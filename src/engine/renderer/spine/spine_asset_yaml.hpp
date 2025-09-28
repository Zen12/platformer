#pragma once
#include "spine_asset.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {

    template <>
    struct convert<SpineAsset>
    {
        static bool decode(const Node &node, SpineAsset &rhs)
        {
            rhs.skeleton = node["skeleton"].as<std::string>();
            rhs.atlas = node["atlas"].as<std::string>();
            rhs.image = node["image"].as<std::string>();
            rhs.json = node["json"].as<std::string>();
            rhs.spineScale = node["spine_scale"].as<float>();
            rhs.moveAnimationName = node["move_animation_name"].as<std::string>();
            rhs.jumpAnimationName = node["jump_animation_name"].as<std::string>();
            rhs.hitAnimationName = node["hit_animation_name"].as<std::string>();
            rhs.idleAnimationName = node["idle_animation_name"].as<std::string>();
            return true;
        }
    };
}
