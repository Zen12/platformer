#pragma once
#include <string>

#include "node/action/action_serialization.hpp"

class TriggerSetterButtonListenerActionSerialization final : public ActionSerialization {
public:
    std::string ButtonId;
    std::string TriggerName;

    ~TriggerSetterButtonListenerActionSerialization() override = default;
};
