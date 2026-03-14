#pragma once
#include "node/action/action_serialization.hpp"
#include "guid.hpp"

class LoadSceneActionSerialization final : public ActionSerialization {
public:
    Guid SceneGuid;

    ~LoadSceneActionSerialization() override = default;
};
