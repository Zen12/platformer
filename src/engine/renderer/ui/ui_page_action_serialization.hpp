#pragma once
#include "../../fsm/node/action/action_serialiazion.hpp"
#include "../../system/guid.hpp"

class UIPageActionSerialization final : public ActionSerialization {
public:
    Guid UiPageGuid;

    ~UIPageActionSerialization() override = default;
};
