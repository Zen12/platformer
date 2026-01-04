#pragma once
#include "../condition_serialization.hpp"

struct AlwaysTrueConditionSerialization final : public ConditionSerialization {
    ~AlwaysTrueConditionSerialization() override = default;
};
