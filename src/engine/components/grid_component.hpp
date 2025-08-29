#pragma once
#include "entity.hpp"


class GridComponent final: public Component {

public:
    std::vector<std::vector<int>> Grid{};
    glm::vec2 SpawnOffset{};
    glm::vec2 SpawnStep{};

    explicit GridComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

    }

    void SetGrid(const std::vector<std::vector<int>> &grid) noexcept{
        this->Grid = grid;
    }

    void SetSpawnOffset(const glm::vec2 &offset) noexcept {
        this->SpawnOffset = offset;
    }

    void SetSpawnStep(const glm::vec2 &step) noexcept {
        this->SpawnStep = step;
    }
};
