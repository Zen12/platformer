#pragma once
#include <entt/entity/registry.hpp>
#include "health_component.hpp"
#include "../../entity/destroy_request_component.hpp"


class HealthSystem {
public:
    HealthSystem();

    void Update(std::shared_ptr<entt::registry> &registry);
};
