#pragma once
#include <string>

#include "node/action/action_serialiazion.hpp"

class ButtonListenerActionSerialization final : public ActionSerialization {
public:
    std::string ButtonId;

    ~ButtonListenerActionSerialization() override = default;
};
