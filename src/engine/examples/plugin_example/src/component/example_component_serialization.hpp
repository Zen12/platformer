#pragma once
#include <string>
#include "entity/component_serialization.hpp"

struct ExampleTagComponentSerialization final : public ComponentSerialization {
    std::string Tag;
    ~ExampleTagComponentSerialization() override = default;
};
