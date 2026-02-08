#pragma once
#include "../entity/component_serialization.hpp"
#include "../system/guid.hpp"

struct BehaviorTreeComponentSerialization final : public ComponentSerialization {
    Guid TreeGuid;
    uint8_t TickPriority = 0;
};
