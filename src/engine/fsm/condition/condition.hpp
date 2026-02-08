#pragma once
#include <string>
#include <variant>

#include "core_types/trigger_check_condition.hpp"
#include "core_types/always_true_condition.hpp"

using AllConditionVariants = std::variant<AlwaysTrueCondition, TriggerCheckCondition>;
