#pragma once
#include <utility>

#include "../components/entity.hpp"


class PrefabSpawner final : public Component {
private:
    float _timer{};
    float _spawnTime{1};
    int _maxCount{1};

    int _currentSpawnCount{0};
    std::weak_ptr<Scene> _scene;
    std::string _prefabId;
    std::vector<glm::vec3> _prefabPositions{};

public:
    explicit PrefabSpawner(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
        _timer = _spawnTime;
    }

    void SetScene(const std::weak_ptr<Scene> &scene) noexcept {
        _scene = scene;
    }

    void SetMaxCount(const int &count) noexcept {
        _maxCount = count;
    }

    void SetSpawnTimer(const float time) noexcept {
        _spawnTime = time;
        _timer = time;
    }

    void SetPrefabPositions(const std::vector<glm::vec3> &positions) noexcept {
        _prefabPositions = positions;
    }

    void SetPrefabId(std::string prefabId) noexcept {
        _prefabId = std::move(prefabId);
    }

    void SpawnOnPosition(const glm::vec3 &position) const {
        if (const auto scene = _scene.lock()) {
            scene->PrefabRequestInstantiate.push_back({
                .Id = _prefabId,
                .Position = position,
                .Rotation = glm::vec3(0.0f, 0.0f, 0.0f),
                .Scale = glm::vec3(1.0f, 1.0f, 1.0f),
            });
        }
    }

    void Spawn() {
        if (_maxCount > 0) {
            _currentSpawnCount++;

            if (_currentSpawnCount > _maxCount) {
                return;
            }
        }

        if (_prefabPositions.empty()) {
            if (const auto e = _entity.lock()) {
                if (const auto tr = e->GetComponent<Transform>().lock()) {
                    SpawnOnPosition(tr->GetPosition());
                }
            }
        } else {
            for (const auto &position: _prefabPositions) {
                SpawnOnPosition(position);
            }
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
