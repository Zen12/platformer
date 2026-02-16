#pragma once
#include "core_types/trigger_check_condition.hpp"
#include "core_types/always_true_condition.hpp"
#include "../type_list.hpp"

using CoreConditionTypes = fsm::TypeList<
    AlwaysTrueCondition,
    TriggerCheckCondition
>;
