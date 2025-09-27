#pragma once
#include "../asset/serialization/serialization_component.hpp"

struct PathFinderSerialization final : public ComponentSerialization {
    std::string gridTag;
};