#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <functional>

struct AStarNode {
    glm::vec2 pos;
    float g = 0.0f;
    float h = 0.0f;
    float f = 0.0f;
    glm::vec2 parent{-1.0f, -1.0f};
};

struct AStarConfig {
    bool allow_diagonal = true;
    bool cut_corners = false;
};

struct AStarPath {
    bool found = false;
    float cost = 0.0f;
    std::vector<glm::vec2> points;
};

class AStarGrid {
public:
    using WalkableFn = std::function<bool(const glm::vec2&)>;

    static AStarPath find_path(
        int width, int height,
        glm::vec2 start,
        glm::vec2 goal,
        const WalkableFn& is_walkable,
        const AStarConfig& cfg = {}
    );
};
