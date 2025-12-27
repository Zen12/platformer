#pragma once
#include "../../entity/component_serialization.hpp"
#include <string>

struct OzzSkeletonComponentSerialization final : public ComponentSerialization {
    std::string SkeletonGuid;  // GUID of the skeleton asset

    ~OzzSkeletonComponentSerialization() override = default;
};
