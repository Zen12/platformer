#pragma once
#include "../entity/component_serialization.hpp"
#include <string>

struct BehaviorTreeComponentSerialization final : public ComponentSerialization {
    std::string TreeGuid;       // GUID of the .bt file
    uint8_t TickPriority = 0;   // LOD: 0=every frame, 1=every 2nd, etc.
};
