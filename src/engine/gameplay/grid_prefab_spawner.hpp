#pragma once
#include "../components/entity.hpp"
#include "../components/grid_component.hpp"


class GridPrefabSpawner final : public Component {
private:
    std::weak_ptr<Scene> _scene{};
    std::string _prefabId{};

private:
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
public:
    explicit GridPrefabSpawner(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

   void SetScene(const std::weak_ptr<Scene> &scene) noexcept {
       _scene = scene;
   }

    void SetPrefabId(std::string prefabId) noexcept {
        _prefabId = std::move(prefabId);
    }

    void Spawn(const std::weak_ptr<GridComponent> &gridComponent) const noexcept {

        if (const auto g = gridComponent.lock()) {
            const auto grid = g->Grid;
            const auto offset = g->SpawnOffset;
            const auto step = g->SpawnStep;

            for (size_t i = 0; i < grid.size(); i++) {
                for (size_t j = 0; j < grid[i].size(); j++) {
                    if (grid[i][j]) {
                        const auto position = glm::vec3(
                            offset.x + step.x * static_cast<float>(j),
                            offset.y + step.y * static_cast<float>(grid[i].size() - i),
                            0);
                        SpawnOnPosition(position);
                    }
                }
            }
        }
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

    }
};
