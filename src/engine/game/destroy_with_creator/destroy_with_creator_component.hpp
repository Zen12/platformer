#pragma once
#include "../../components/entity.hpp"
#include "../../scene/scene.hpp"

class DestroyWithCreatorComponent final: public Component {
private:
    std::weak_ptr<Scene> _scene;

public:
    explicit DestroyWithCreatorComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SetScene(const std::weak_ptr<Scene> &scene) noexcept {
        _scene = scene;
    }

    void Update([[maybe_unused]] const float &deltaTime) override {
        if (const auto entity = _entity.lock()) {
            if (const auto &scene = _scene.lock()) {
                const auto creatorId = entity->GetCreator();
                if (!creatorId.empty()) {
                    const auto creator = scene->GetEntityById(creatorId);
                    if (creator.expired()) {
                        scene->RemoveEntityById(entity->GetId());
                    }
                }
            }
        }
    }
};