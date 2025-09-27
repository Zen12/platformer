#pragma once
#include "../../components/entity.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

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

    glm::vec3 GetPositionFromIndex(const int &x, const int &y) const noexcept  {
        return glm::vec3(
            SpawnOffset.x + SpawnStep.x * static_cast<float>(y),
            SpawnOffset.y + SpawnStep.y * static_cast<float>(Grid[x].size() - x),
            0);
    }

    glm::ivec2 GetClosestIndexFromPosition(const glm::vec2 &pos) const noexcept {
        // Reverse the formula used in GetPositionFromIndex
        int y = static_cast<int>(std::round((pos.x - SpawnOffset.x) / SpawnStep.x));

        int x = static_cast<int>(Grid.size()); // default if empty
        if (!Grid.empty()) {
            // Solve for x using the inverted formula
            x = static_cast<int>(Grid[0].size() -
                std::round((pos.y - SpawnOffset.y) / SpawnStep.y));
        }

        return glm::ivec2(x, y);
    }
};