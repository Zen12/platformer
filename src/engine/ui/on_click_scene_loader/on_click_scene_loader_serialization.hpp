#pragma once

#include "../../entity/component_serialization.hpp"

struct OnClickSceneLoaderSerialization final : public ComponentSerialization {
    std::string SceneGuid;
};