#pragma once
#include "../../entity/component_serialization.hpp"
#include "guid.hpp"
#include "animation_blender_config.hpp"

struct FsmAnimationComponentSerialization final : public ComponentSerialization {
    Guid FsmGuid;
    bool Loop = true;
    AnimationBlenderConfig BlenderConfig;
};
