#pragma once
#include "esc/esc_core.hpp"
#include "../component/example_component.hpp"

// Example ECS system that iterates entities with ExampleTagComponent
// Registered with priority 900 (runs after most systems)
class ExampleSystem final : public ISystem {
    entt::registry& _registry;

public:
    explicit ExampleSystem(entt::registry& registry) : _registry(registry) {}

    void OnTick() override {
        auto view = _registry.view<ExampleTagComponent>();
        for (auto entity : view) {
            [[maybe_unused]] const auto& tag = view.get<ExampleTagComponent>(entity);
            // Example: do something with tagged entities each frame
        }
    }
};
