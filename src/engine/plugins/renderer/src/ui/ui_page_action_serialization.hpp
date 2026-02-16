#pragma once
#include "node/action/action_serialiazion.hpp"
#include "guid.hpp"

class UIPageActionSerialization final : public ActionSerialization {
public:
    Guid UiPageGuid;

    ~UIPageActionSerialization() override = default;
};
