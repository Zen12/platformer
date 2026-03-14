#pragma once
#include "node/action/action_serialization.hpp"
#include "guid.hpp"

class UIPageActionSerialization final : public ActionSerialization {
public:
    Guid UiPageGuid;

    ~UIPageActionSerialization() override = default;
};
