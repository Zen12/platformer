#pragma once
#include "entity/component_serialization.hpp"

struct CombatStateComponentSerialization final : public ComponentSerialization {
    ~CombatStateComponentSerialization() override = default;
};
