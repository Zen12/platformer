#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <glm/glm.hpp>

struct Node {
    glm::ivec2 pos{-1, -1};
    int g{0};
    int f{0};
    glm::ivec2 parent{-1, -1};
};

class AStar {
public:
    AStar(const int &width, const int &height, const std::vector<std::vector<int>>& grid)
        : width(width), height(height), grid(grid),
          nodes(height, std::vector<Node>(width)),
          closed(height, std::vector<bool>(width, false)) {}

    std::vector<glm::ivec2> findPath(glm::ivec2 start, glm::ivec2 goal, bool canJump) {
        auto heuristic = [&](const glm::ivec2& a, const glm::ivec2& b) {
            return abs(a.x - b.x) + abs(a.y - b.y);
        };

        auto cmp = [](const Node& a, const Node& b) { return a.f > b.f; };
        std::priority_queue<Node, std::vector<Node>, decltype(cmp)> open(cmp);

        // Clear previous search
        for (auto& row : nodes) for (auto& n : row) n = Node{};
        for (auto& row : closed) std::fill(row.begin(), row.end(), false);

        Node startNode{start, 0, heuristic(start, goal), {-1,-1}};
        nodes[start.x][start.y] = startNode;
        open.push(startNode);

        const auto& dirs = GetDirs(canJump);

        while (!open.empty()) {
            Node current = open.top();
            open.pop();

            if (current.pos == goal) {
                return ReconstructPath(goal);
            }

            closed[current.pos.x][current.pos.y] = true;

            for (auto& d : dirs) {
                glm::ivec2 next = current.pos + d;

                if (!InBounds(next) || closed[next.x][next.y] || !IsWalkable(next)) continue;

                const int newG = current.g + 1;
                const int h = heuristic(next, goal);
                const int f = newG + h;

                if (Node& neighborNode = nodes[next.x][next.y]; neighborNode.pos.x == -1 || newG < neighborNode.g) {
                    neighborNode = {next, newG, f, current.pos};
                    open.push(neighborNode);
                }
            }
        }

        return {}; // no path
    }

private:
    int width, height;
    std::vector<std::vector<int>> grid;
    std::vector<std::vector<Node>> nodes;
    std::vector<std::vector<bool>> closed;

    [[nodiscard]] bool InBounds(const glm::ivec2& p) const {
        return p.x >= 0 && p.y >= 0 && p.x < height && p.y < width;
    }

    [[nodiscard]] bool IsWalkable(const glm::ivec2& p) const {
        return grid[p.x][p.y] == 0;
    }

    [[nodiscard]] static const std::vector<glm::ivec2>& GetDirs(const bool &canJump) {
        static std::vector<glm::ivec2> dirsJump{{1,0},{-1,0},{0,1},{0,-1}};
        static std::vector<glm::ivec2> dirsNoJump{{0,-1},{1,0},{0,1}};
        return canJump ? dirsJump : dirsNoJump;
    }

    [[nodiscard]] std::vector<glm::ivec2> ReconstructPath(const glm::ivec2& goal) const {
        std::vector<glm::ivec2> path;
        glm::ivec2 p = goal;

        while (true) {
            path.push_back(p);
            const Node& n = nodes[p.x][p.y];
            if (n.parent.x == -1) break;
            p = n.parent;
        }

        std::reverse(path.begin(), path.end());
        return path;
    }
};
