#pragma once
#include <memory>

#include "astar_grid.hpp"
#include "../scene/scene.hpp"

class AStarFinder{

private:
    std::weak_ptr<Scene> _scene;
    std::unique_ptr<AStarGrid> _astarGrid;

public:
    AStarFinder() {
        _astarGrid = std::make_unique<AStarGrid>();
    }

    void SetScene(const std::weak_ptr<Scene> &scene) {
        _scene = scene;
    }

    void Initialize() {
        const int W = 10, H = 8;

        // blocked cells
        std::vector<bool> blocked(W * H, false);
        auto idx = [&](const int x, const int y){ return y * W + x; };
        blocked[idx(4,3)] = true;
        blocked[idx(4,4)] = true;
        blocked[idx(4,5)] = true;

        auto is_walkable = [&](const glm::vec2& p) {
            int x = (int)std::round(p.x);
            int y = (int)std::round(p.y);
            if (x < 0 || y < 0 || x >= W || y >= H) return false;
            return !blocked[idx(x, y)];
        };

        AStarConfig cfg;
        cfg.allow_diagonal = true;
        cfg.cut_corners = false;

        glm::vec2 start{0,0};
        glm::vec2 goal{9,7};
        AStarPath path = AStarGrid::find_path(W, H, start, goal, is_walkable, cfg);

        if (path.found) {
            std::cout << "Path cost = " << path.cost << "\n";
            for (auto& p : path.points) {
                std::cout << "(" << p.x << "," << p.y << ")\n";
            }
        } else {
            std::cout << "No path found\n";
        }
    }


};
