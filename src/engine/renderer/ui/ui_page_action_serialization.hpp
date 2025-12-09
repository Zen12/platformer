#pragma once
#include <string>

#include "../../fsm/node/action/action_serialiazion.hpp"

class UIPageActionSerialization final : public ActionSerialization {
public:
    std::string UiPageGuid;

    ~UIPageActionSerialization() override = default;
};
