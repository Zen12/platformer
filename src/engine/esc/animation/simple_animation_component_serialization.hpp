#pragma once
#include "../../entity/component_serialization.hpp"
#include <string>

struct SimpleAnimationComponentSerialization final : public ComponentSerialization {
    std::string AnimationGuid;
    bool ApplyRootMotion = false;
    std::string RootBoneName = "mixamorig:Hips_01";

    ~SimpleAnimationComponentSerialization() override = default;
};
