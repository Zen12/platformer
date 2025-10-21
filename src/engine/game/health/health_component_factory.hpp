#pragma once
#include <utility>

#include "health_component.hpp"
#include "health_component_serialization.hpp"


class HealthComponentFactory {
private:
    std::shared_ptr<entt::registry> _registry;

public:
    explicit HealthComponentFactory(std::shared_ptr<entt::registry> registry)
    : _registry(std::move(registry))
    {}

    void FillComponent(const entt::entity &e, const HealthComponentSerialization &serialization) const {
        _registry->emplace<HealthComponent>(e, serialization.Health, serialization.Health);
    }
};