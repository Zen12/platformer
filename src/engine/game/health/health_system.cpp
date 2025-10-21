#include "health_system.hpp"


HealthSystem::HealthSystem() {
}

void HealthSystem::Update(std::shared_ptr<entt::registry> &registry) {
    const auto view = registry->view<HealthComponent>();
    for (const auto &entity: view) {
        const auto health = view.get<HealthComponent>(entity);
        if (health.GetHealth() <= 0.0f ) {
            registry->emplace<DestroyRequestComponent>(entity);
        }
    }
}
