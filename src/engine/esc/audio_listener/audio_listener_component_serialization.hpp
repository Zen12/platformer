#pragma once
#include "../../entity/component_serialization.hpp"

struct AudioListenerComponentSerialization final : public ComponentSerialization {
    // AudioListener is a marker component, no additional data needed
};
