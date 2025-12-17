#pragma once
#include <string>

#include "../../fsm/node/action/action_serialiazion.hpp"

class TriggerSetterButtonListenerActionSerialization final : public ActionSerialization {
public:
    std::string ButtonId;
    std::string TriggerName;

    ~TriggerSetterButtonListenerActionSerialization() override = default;
};
