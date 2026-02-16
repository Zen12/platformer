#pragma once
#include "node/action/action_serialiazion.hpp"
#include "guid.hpp"

class LoadSceneActionSerialization final : public ActionSerialization {
public:
    Guid SceneGuid;

    ~LoadSceneActionSerialization() override = default;
};
