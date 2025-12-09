#pragma once
#include "../condition_serialization.hpp"


class BoolCheckConditionSerialization final : public ConditionSerialization {
public:

    std::string ValueName{};
    bool Value{};

    ~BoolCheckConditionSerialization() override = default;
};