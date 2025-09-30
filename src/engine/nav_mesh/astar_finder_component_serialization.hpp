#pragma once
#include "../entity/component_serialization.hpp"

struct PathFinderSerialization final : public ComponentSerialization {
    std::string gridTag;
};