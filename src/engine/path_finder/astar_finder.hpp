#pragma once
#include <memory>

#include "astar_grid.hpp"
#include "../scene/scene.hpp"

class AStarFinderComponent final : public Component {

private:
    std::unique_ptr<AStarGrid> _astarGrid{};
    std::vector<bool> _blocked{};
    int _width{}, _height{};
    std::function<int(int, int)> idx = [&](const int x, const int y){ return y * _width + x; };
    AStarConfig _cfg{ .allow_diagonal = true, .cut_corners = false };
    std::weak_ptr<GridComponent> _gridComponent;

public:

    explicit AStarFinderComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void Initialize(const std::weak_ptr<GridComponent> &gridComponent) {
        _gridComponent = gridComponent;
        if (const auto gridRef = gridComponent.lock()) {
            const auto grid = gridRef->Grid;

            _width = grid[0].size();
            _height = grid.size();

            // blocked cells
            _blocked.clear();
            _blocked.resize(_width * _height, false);

            for (size_t i = 0; i < grid.size(); i++) {
                for (size_t j = 0; j < grid[i].size(); j++) {
                    const auto value = grid[i][j];
                    _blocked[idx(i, j)] = value == 1;
                }
            }
        }

    }


    AStarPath GetPath(const glm::vec2 start, const glm::vec2 goal) const {
        AStarGrid::WalkableFn isWalkable = [this](const glm::vec2& pos) {
            if (const auto grid = _gridComponent.lock()) {
                const auto indexPos = grid->GetClosestIndexFromPosition(pos);

                if (indexPos.x < 0 || indexPos.y < 0 || indexPos.x >= _width || indexPos.y >= _height)
                    return false;

                return !_blocked[idx(indexPos.x, indexPos.y)];
            }

            return false;
        };

        return AStarGrid::find_path(_width, _height, start, goal, isWalkable, _cfg);
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

    }
};
