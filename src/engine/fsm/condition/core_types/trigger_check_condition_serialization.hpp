#pragma once
#include "../condition_serialization.hpp"


class TriggerCheckConditionSerialization final : public ConditionSerialization {
public:

    std::string TriggerName{};

    ~TriggerCheckConditionSerialization() override = default;
};