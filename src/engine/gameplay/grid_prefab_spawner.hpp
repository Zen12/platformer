#pragma once
#include "../components/entity.hpp"
#include "../scene/scene.hpp"


class GridPrefabSpawner final : public Component {
private:
    std::weak_ptr<Scene> _scene{};
    std::string _prefabId{};
    glm::vec3 _prefabSpawnStep{};
    glm::vec3 _prefabSpawnOffset{};
    std::vector<std::vector<bool>> _grid{};
public:
    explicit GridPrefabSpawner(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SpawnOnPosition(const glm::vec3 &position) const noexcept {
        if (const auto scene = _scene.lock()) {
            scene->PrefabRequestInstantiate.push_back({
                .Id = _prefabId,
                .Position = position,
                .Rotation = glm::vec3(0.0f, 0.0f, 0.0f),
                .Scale = glm::vec3(1.0f, 1.0f, 1.0f),
            });
        }
    }

    void SetSpawnStep(const glm::vec3 &step) noexcept {
        _prefabSpawnStep = step;
    }

    void SetSpawnOffset(const glm::vec3 &offset) noexcept {
        _prefabSpawnOffset = offset;
    }

    void SetScene(const std::weak_ptr<Scene> &scene) noexcept {
        _scene = scene;
    }


    void SetPrefabId(std::string prefabId) noexcept {
        _prefabId = std::move(prefabId);
    }

    void Spawn(std::vector<std::vector<bool>> grid) noexcept {

        _grid = grid;
        for (size_t i = 0; i < grid.size(); i++) {
            for (size_t j = 0; j < grid[i].size(); j++) {
                if (grid[i][j]) {
                    const auto position = glm::vec3(
                        _prefabSpawnOffset.x + _prefabSpawnStep.x * static_cast<float>(j),
                        _prefabSpawnOffset.y + _prefabSpawnStep.y * static_cast<float>(grid[i].size() - i),
                        0);
                    SpawnOnPosition(position);
                }
            }
        }
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

    }
};
