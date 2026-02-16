#pragma once
#include "type_list.hpp"
#include "condition/core_condition_types.hpp"

namespace fsm {

using AllConditionTypes = Concat_t<CoreConditionTypes>;
using AllConditionVariants = ToVariant_t<AllConditionTypes>;

}
