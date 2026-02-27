#pragma once
#include "condition/condition_serialization.hpp"

struct ExampleAlwaysPassConditionSerialization final : public ConditionSerialization {
    ~ExampleAlwaysPassConditionSerialization() override = default;
};
