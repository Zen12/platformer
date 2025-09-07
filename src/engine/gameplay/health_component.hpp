#pragma once

#include "../components/entity.hpp"
#include "../scene/scene.hpp"

class HealthComponent final : public Component {
private:
    float _maxHealth;
    float _health;
    std::weak_ptr<Scene> _scene;
public:
    explicit HealthComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity), _maxHealth(0), _health(0) {
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

    }

    void SetHealth(const float &health) noexcept {
        _health = health;
    }

    void SetMaxHealth(const float &maxHealth) noexcept {
        _maxHealth = maxHealth;
    }

    float GetPercentageHealth() const noexcept {
        if (_health <= 0)
            return 0;

        if (_maxHealth <= 0)
            return 0;

        return _health / _maxHealth;
    }

    void SetScene(const std::weak_ptr<Scene> &scene) {
        _scene = scene;
    }

    void DecreaseHealth(const float &health) {
        _health -= health;
        if (_health <= 0) {
            if (const auto scene = _scene.lock()) {
                if (const auto entity = GetEntity().lock())
                scene->RemoveEntityById(entity->GetId());
            }
        }
    }
};
