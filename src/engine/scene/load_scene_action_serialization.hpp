#pragma once
#include <string>

#include "../fsm/node/action/action_serialiazion.hpp"

class LoadSceneActionSerialization final : public ActionSerialization {
public:
    std::string SceneGuid;

    ~LoadSceneActionSerialization() override = default;
};
