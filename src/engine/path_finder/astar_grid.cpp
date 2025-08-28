#include "astar_grid.hpp"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <limits>

struct Vec2Hash {
    size_t operator()(const glm::vec2& v) const noexcept {
        auto x = static_cast<int>(std::round(v.x));
        auto y = static_cast<int>(std::round(v.y));
        return (static_cast<size_t>(x) << 32) ^ static_cast<size_t>(y);
    }
};

struct Vec2Eq {
    bool operator()(const glm::vec2& a, const glm::vec2& b) const noexcept {
        return std::round(a.x) == std::round(b.x) &&
               std::round(a.y) == std::round(b.y);
    }
};

AStarPath AStarGrid::find_path(
    int width, int height,
    glm::vec2 start,
    glm::vec2 goal,
    const WalkableFn& is_walkable,
    const AStarConfig& cfg
) {
    auto heuristic = [](const glm::vec2& a, const glm::vec2& b) {
        float dx = std::abs(a.x - b.x);
        float dy = std::abs(a.y - b.y);
        return (dx + dy) + (std::sqrt(2.0f) - 2.0f) * std::min(dx, dy);
    };

    std::vector<glm::vec2> directions{
        {1,0}, {-1,0}, {0,1}, {0,-1}
    };
    if (cfg.allow_diagonal) {
        directions.insert(directions.end(), {
            {1,1}, {1,-1}, {-1,1}, {-1,-1}
        });
    }

    auto cmp = [](const AStarNode* a, const AStarNode* b) {
        return a->f > b->f;
    };

    std::priority_queue<AStarNode*, std::vector<AStarNode*>, decltype(cmp)> open(cmp);
    std::unordered_map<glm::vec2, AStarNode, Vec2Hash, Vec2Eq> nodes;
    std::unordered_set<glm::vec2, Vec2Hash, Vec2Eq> closed;

    nodes[start] = {start, 0.0f, heuristic(start, goal), heuristic(start, goal), {-1,-1}};
    open.push(&nodes[start]);

    while (!open.empty()) {
        AStarNode* current = open.top();
        open.pop();

        if (Vec2Eq{}(current->pos, goal)) {
            // reconstruct path
            AStarPath result;
            result.found = true;
            result.cost = current->g;
            glm::vec2 p = goal;
            while (p.x >= 0.0f && p.y >= 0.0f) {
                result.points.push_back(p);
                p = nodes[p].parent;
            }
            std::reverse(result.points.begin(), result.points.end());
            return result;
        }

        closed.insert(current->pos);

        for (auto& d : directions) {
            glm::vec2 next = current->pos + d;

            if (next.x < 0 || next.y < 0 || next.x >= width || next.y >= height)
                continue;

            if (!is_walkable(next)) continue;

            if (cfg.allow_diagonal && !cfg.cut_corners && std::abs(d.x) + std::abs(d.y) == 2) {
                glm::vec2 n1{current->pos.x + d.x, current->pos.y};
                glm::vec2 n2{current->pos.x, current->pos.y + d.y};
                if (!is_walkable(n1) || !is_walkable(n2)) continue;
            }

            if (closed.count(next)) continue;

            float step_cost = (std::abs(d.x) + std::abs(d.y) == 2) ? std::sqrt(2.0f) : 1.0f;
            float tentative_g = current->g + step_cost;

            auto it = nodes.find(next);
            if (it == nodes.end() || tentative_g < it->second.g) {
                float h = heuristic(next, goal);
                nodes[next] = {next, tentative_g, h, tentative_g + h, current->pos};
                open.push(&nodes[next]);
            }
        }
    }

    return {}; // not found
}
