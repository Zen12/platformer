#pragma once
#include <utility>

#include "../components/entity.hpp"


class PrefabSpawner final : public Component {
private:
    float _timer{};
    float _spawnTime{1}; // serialize
    std::weak_ptr<Scene> _scene;
    std::string _prefabId;

public:
    explicit PrefabSpawner(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
        _timer = _spawnTime;
    }

    void SetScene(const std::weak_ptr<Scene> &scene) {
        _scene = scene;
    }

    void SetSpawnTimer(const float time) noexcept {
        _spawnTime = time;
    }

    void SetPrefabId(std::string prefabId) noexcept {
        _prefabId = std::move(prefabId);
    }

    void Spawn() const {
        if (const auto scene = _scene.lock()) {
            scene->PrefabRequestInstantiate.push_back({
                .Id = _prefabId,
                .Position = glm::vec3(0.0f, 0.0f, 0.0f),
                .Rotation = glm::vec3(0.0f, 0.0f, 0.0f),
                .Scale = glm::vec3(1.0f, 1.0f, 1.0f),
            });
        }
    }

    void Update(const float &deltaTime) override {
        _timer -= deltaTime;
        if (_timer <= 0) {
            _timer = _spawnTime;
            Spawn();
        }
    }
};
