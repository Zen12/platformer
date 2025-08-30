#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <glm/glm.hpp>

// Hash + equality for glm::ivec2
struct IVec2Hash {
    std::size_t operator()(const glm::ivec2& v) const noexcept {
        return std::hash<int>()(v.x * 73856093 ^ v.y * 19349663);
    }
};
struct IVec2Eq {
    bool operator()(const glm::ivec2& a, const glm::ivec2& b) const noexcept {
        return a.x == b.x && a.y == b.y;
    }
};


struct Node {
    glm::ivec2 pos;
    int g, f;
    glm::ivec2 parent;
};

class AStar {
private:
    std::vector<glm::ivec2> GetDirs(const bool &canJump) {
        if (canJump) {
            return { {1,0}, {-1,0}, {0,1}, {0,-1}};
        }

        return { {0,-1}, {1,0}, {0,1} };
    }
public:
    AStar(int width, int height, const std::vector<std::vector<int>>& grid)
        : width(width), height(height), grid(grid) {}

    std::vector<glm::ivec2> findPath(glm::ivec2 start, glm::ivec2 goal, bool canJump) {
        auto heuristic = [&](const glm::ivec2& a, const glm::ivec2& b) {
            return abs(a.x - b.x) + abs(a.y - b.y); // Manhattan
        };

        auto cmp = [](const Node& a, const Node& b) { return a.f > b.f; };
        std::priority_queue<Node, std::vector<Node>, decltype(cmp)> open(cmp);

        std::unordered_map<glm::ivec2, Node, IVec2Hash, IVec2Eq> nodes;
        std::unordered_map<glm::ivec2, bool, IVec2Hash, IVec2Eq> closed;

        Node startNode{start, 0, heuristic(start, goal), {-1, -1}};
        open.push(startNode);
        nodes[start] = startNode;

        const std::vector<glm::ivec2> dirs =GetDirs(canJump);


        while (!open.empty()) {
            Node current = open.top();
            open.pop();

            if (current.pos == goal) {
                return reconstructPath(nodes, goal);
            }

            closed[current.pos] = true;

            for (auto& d : dirs) {
                glm::ivec2 next = current.pos + d;

                if (!inBounds(next) || !isWalkable(next)) continue;
                if (closed[next]) continue;

                int newG = current.g + 1;
                int h = heuristic(next, goal);
                int f = newG + h;

                auto it = nodes.find(next);
                if (it == nodes.end() || newG < it->second.g) {
                    Node neighbor{next, newG, f, current.pos};
                    nodes[next] = neighbor;
                    open.push(neighbor);
                }
            }
        }

        return {}; // no path
    }

private:
    int width, height;
    std::vector<std::vector<int>> grid;

    bool inBounds(const glm::ivec2& p) const {
        return p.x >= 0 && p.y >= 0 && p.x < height && p.y < width;
    }

    bool isWalkable(const glm::ivec2& p) const {
        return grid[p.x][p.y] == 0; // 0 = walkable, 1 = obstacle
    }

    std::vector<glm::ivec2> reconstructPath(
        const std::unordered_map<glm::ivec2, Node, IVec2Hash, IVec2Eq>& nodes,
        glm::ivec2 goal
    ) {
        std::vector<glm::ivec2> path;
        glm::ivec2 p = goal;
        while (true) {
            path.push_back(p);
            auto it = nodes.find(p);
            if (it == nodes.end() || it->second.parent.x < 0) break;
            p = it->second.parent;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

};
