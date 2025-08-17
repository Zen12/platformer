#pragma once

#include "../components/entity.hpp"
#include "../scene/scene.hpp"

class HealthComponent final : public Component {
private:
    float _health;
    std::weak_ptr<Scene> _scene;
public:
    explicit HealthComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity), _health(0) {
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

    }

    void SetHealth(const float &health) {
        _health = health;
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
