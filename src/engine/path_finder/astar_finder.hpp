#pragma once
#include <memory>

#include "astar_grid.hpp"
#include "../scene/scene.hpp"

class AStarFinderComponent final : public Component {

private:
    std::unique_ptr<AStar> _astarGrid{};
    std::vector<bool> _blocked{};
    int _width{}, _height{};
    std::function<int(int, int)> idx = [&](const int x, const int y){ return y * _width + x; };
    std::weak_ptr<GridComponent> _gridComponent;

public:

    explicit AStarFinderComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void Initialize(const std::weak_ptr<GridComponent> &gridComponent) {
        _gridComponent = gridComponent;
        if (const auto gridRef = gridComponent.lock()) {
            const auto gridValue = gridRef->Grid;
            _astarGrid = std::make_unique<AStar>(gridValue[0].size(), gridValue.size(), gridValue);
        }
    }


    [[nodiscard]] std::vector<glm::ivec2> GetPath(const glm::vec2 start, const glm::vec2 goal, const bool &canJump) const {

        if (const auto grid = _gridComponent.lock()) {
            const auto startI = grid->GetClosestIndexFromPosition(start);
            const auto endI = grid->GetClosestIndexFromPosition(goal);
            return _astarGrid->findPath(startI, endI, canJump);
        }
        return {};
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

    }
};
