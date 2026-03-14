#pragma once
#include <string>

#include "node/action/action_serialization.hpp"

class ButtonListenerActionSerialization final : public ActionSerialization {
public:
    std::string ButtonId;

    ~ButtonListenerActionSerialization() override = default;
};
