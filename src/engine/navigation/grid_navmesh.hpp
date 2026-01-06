#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <queue>
#include <unordered_set>
#include <cmath>
#include <algorithm>

class GridNavmesh {
private:
    std::vector<std::vector<int>> _grid;
    int _width = 0;
    int _height = 0;
    float _cellSize = 1.0f;
    glm::vec3 _origin = glm::vec3(0.0f);

    struct Node {
        int x, z;
        float g, h;
        int parentX, parentZ;

        float f() const { return g + h; }

        bool operator>(const Node& other) const {
            return f() > other.f();
        }
    };

    [[nodiscard]] float Heuristic(int x1, int z1, int x2, int z2) const {
        // Octile distance (allows diagonal movement)
        const int dx = std::abs(x2 - x1);
        const int dz = std::abs(z2 - z1);
        return static_cast<float>(std::max(dx, dz)) + 0.414f * static_cast<float>(std::min(dx, dz));
    }

    [[nodiscard]] bool IsValidCell(int x, int z) const {
        return x >= 0 && x < _width && z >= 0 && z < _height;
    }

    [[nodiscard]] bool IsWalkableCell(int x, int z) const {
        if (!IsValidCell(x, z)) return false;
        if (z >= static_cast<int>(_grid.size())) return false;
        if (x >= static_cast<int>(_grid[z].size())) return false;
        return _grid[z][x] != 0;
    }

    [[nodiscard]] glm::vec3 GridToWorld(int x, int z) const {
        return glm::vec3(
            _origin.x + (static_cast<float>(x) + 0.5f) * _cellSize,
            _origin.y,
            _origin.z + (static_cast<float>(z) + 0.5f) * _cellSize
        );
    }

    void WorldToGrid(const glm::vec3& pos, int& outX, int& outZ) const {
        outX = static_cast<int>(std::floor((pos.x - _origin.x) / _cellSize));
        outZ = static_cast<int>(std::floor((pos.z - _origin.z) / _cellSize));
    }

    // Smooth path by removing unnecessary waypoints (line-of-sight check)
    [[nodiscard]] std::vector<glm::vec3> SmoothPath(const std::vector<glm::vec3>& path) const {
        if (path.size() <= 2) return path;

        std::vector<glm::vec3> smoothed;
        smoothed.push_back(path[0]);

        size_t current = 0;
        while (current < path.size() - 1) {
            size_t furthest = current + 1;

            // Find furthest visible point
            for (size_t i = current + 2; i < path.size(); ++i) {
                if (HasLineOfSight(path[current], path[i])) {
                    furthest = i;
                }
            }

            smoothed.push_back(path[furthest]);
            current = furthest;
        }

        return smoothed;
    }

    // Check if there's a clear line between two points (Bresenham)
    [[nodiscard]] bool HasLineOfSight(const glm::vec3& from, const glm::vec3& to) const {
        int x0, z0, x1, z1;
        WorldToGrid(from, x0, z0);
        WorldToGrid(to, x1, z1);

        const int dx = std::abs(x1 - x0);
        const int dz = std::abs(z1 - z0);
        const int sx = x0 < x1 ? 1 : -1;
        const int sz = z0 < z1 ? 1 : -1;
        int err = dx - dz;

        while (true) {
            if (!IsWalkableCell(x0, z0)) return false;

            if (x0 == x1 && z0 == z1) break;

            const int e2 = 2 * err;
            if (e2 > -dz) {
                err -= dz;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                z0 += sz;
            }
        }

        return true;
    }

public:
    GridNavmesh() = default;

    bool Initialize(int width, int height, float cellSize, const glm::vec3& origin,
                   const std::vector<std::vector<int>>& walkabilityGrid) {
        _width = width;
        _height = height;
        _cellSize = cellSize;
        _origin = origin;
        _grid = walkabilityGrid;
        return !_grid.empty();
    }

    [[nodiscard]] std::vector<glm::vec3> FindPath(const glm::vec3& start, const glm::vec3& end) const {
        int startX, startZ, endX, endZ;
        WorldToGrid(start, startX, startZ);
        WorldToGrid(end, endX, endZ);

        // Clamp to valid range
        startX = std::clamp(startX, 0, _width - 1);
        startZ = std::clamp(startZ, 0, _height - 1);
        endX = std::clamp(endX, 0, _width - 1);
        endZ = std::clamp(endZ, 0, _height - 1);

        if (!IsWalkableCell(startX, startZ) || !IsWalkableCell(endX, endZ)) {
            return {};
        }

        // A* with priority queue
        std::priority_queue<Node, std::vector<Node>, std::greater<>> openSet;
        std::vector<std::vector<bool>> closed(_height, std::vector<bool>(_width, false));
        std::vector<std::vector<Node>> nodeData(_height, std::vector<Node>(_width));

        // Initialize start node
        Node startNode{startX, startZ, 0.0f, Heuristic(startX, startZ, endX, endZ), -1, -1};
        openSet.push(startNode);
        nodeData[startZ][startX] = startNode;

        // 8 directions (including diagonals)
        const int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
        const int dz[] = {-1, -1, 0, 1, 1, 1, 0, -1};
        const float costs[] = {1.0f, 1.414f, 1.0f, 1.414f, 1.0f, 1.414f, 1.0f, 1.414f};

        while (!openSet.empty()) {
            Node current = openSet.top();
            openSet.pop();

            if (current.x == endX && current.z == endZ) {
                // Reconstruct path
                std::vector<glm::vec3> path;
                int cx = current.x, cz = current.z;

                while (cx != -1 && cz != -1) {
                    path.push_back(GridToWorld(cx, cz));
                    const Node& n = nodeData[cz][cx];
                    const int px = n.parentX, pz = n.parentZ;
                    cx = px;
                    cz = pz;
                }

                std::reverse(path.begin(), path.end());

                // Replace first point with exact start, last with exact end
                if (!path.empty()) {
                    path[0] = start;
                    path.back() = end;
                }

                return SmoothPath(path);
            }

            if (closed[current.z][current.x]) continue;
            closed[current.z][current.x] = true;

            // Explore neighbors
            for (int i = 0; i < 8; ++i) {
                const int nx = current.x + dx[i];
                const int nz = current.z + dz[i];

                if (!IsWalkableCell(nx, nz) || closed[nz][nx]) continue;

                // For diagonal movement, check that both adjacent cells are walkable
                if (i % 2 == 1) {  // Diagonal
                    if (!IsWalkableCell(current.x + dx[i], current.z) ||
                        !IsWalkableCell(current.x, current.z + dz[i])) {
                        continue;
                    }
                }

                const float newG = current.g + costs[i];

                if (newG < nodeData[nz][nx].g || nodeData[nz][nx].g == 0.0f) {
                    Node neighbor{nx, nz, newG, Heuristic(nx, nz, endX, endZ), current.x, current.z};
                    nodeData[nz][nx] = neighbor;
                    openSet.push(neighbor);
                }
            }
        }

        return {};  // No path found
    }

    [[nodiscard]] glm::vec3 FindNearestPoint(const glm::vec3& position) const {
        int startX, startZ;
        WorldToGrid(position, startX, startZ);

        // If already walkable, return position
        if (IsWalkableCell(startX, startZ)) {
            return position;
        }

        // BFS to find nearest walkable cell
        std::queue<std::pair<int, int>> queue;
        std::vector<std::vector<bool>> visited(_height, std::vector<bool>(_width, false));

        // Start from clamped position
        startX = std::clamp(startX, 0, _width - 1);
        startZ = std::clamp(startZ, 0, _height - 1);

        queue.push({startX, startZ});
        visited[startZ][startX] = true;

        const int dx[] = {0, 1, 0, -1, 1, 1, -1, -1};
        const int dz[] = {-1, 0, 1, 0, -1, 1, 1, -1};

        while (!queue.empty()) {
            auto [x, z] = queue.front();
            queue.pop();

            if (IsWalkableCell(x, z)) {
                return GridToWorld(x, z);
            }

            for (int i = 0; i < 8; ++i) {
                const int nx = x + dx[i];
                const int nz = z + dz[i];

                if (IsValidCell(nx, nz) && !visited[nz][nx]) {
                    visited[nz][nx] = true;
                    queue.push({nx, nz});
                }
            }
        }

        return position;  // Fallback
    }

    [[nodiscard]] bool IsWalkable(const glm::vec3& position) const {
        int x, z;
        WorldToGrid(position, x, z);
        return IsWalkableCell(x, z);
    }

    // Overload for direct grid check
    [[nodiscard]] bool IsWalkable(int x, int z) const {
        return IsWalkableCell(x, z);
    }

    // Getters
    [[nodiscard]] int GetWidth() const noexcept { return _width; }
    [[nodiscard]] int GetHeight() const noexcept { return _height; }
    [[nodiscard]] float GetCellSize() const noexcept { return _cellSize; }
    [[nodiscard]] const glm::vec3& GetOrigin() const noexcept { return _origin; }
    [[nodiscard]] const std::vector<std::vector<int>>& GetGrid() const noexcept { return _grid; }

    // Get boundary edges for RVO2 obstacles (each edge is a line segment)
    [[nodiscard]] std::vector<std::pair<glm::vec3, glm::vec3>> GetBoundaryEdges() const {
        std::vector<std::pair<glm::vec3, glm::vec3>> edges;

        for (int z = 0; z < _height; ++z) {
            for (int x = 0; x < _width; ++x) {
                if (!IsWalkableCell(x, z)) continue;

                // Get cell corners
                const float x0 = _origin.x + static_cast<float>(x) * _cellSize;
                const float x1 = _origin.x + static_cast<float>(x + 1) * _cellSize;
                const float z0 = _origin.z + static_cast<float>(z) * _cellSize;
                const float z1 = _origin.z + static_cast<float>(z + 1) * _cellSize;
                const float y = _origin.y;

                // Add edge if neighbor is non-walkable (CCW winding for RVO2)
                // North edge (z-1)
                if (!IsWalkableCell(x, z - 1)) {
                    edges.emplace_back(glm::vec3(x1, y, z0), glm::vec3(x0, y, z0));
                }
                // South edge (z+1)
                if (!IsWalkableCell(x, z + 1)) {
                    edges.emplace_back(glm::vec3(x0, y, z1), glm::vec3(x1, y, z1));
                }
                // West edge (x-1)
                if (!IsWalkableCell(x - 1, z)) {
                    edges.emplace_back(glm::vec3(x0, y, z0), glm::vec3(x0, y, z1));
                }
                // East edge (x+1)
                if (!IsWalkableCell(x + 1, z)) {
                    edges.emplace_back(glm::vec3(x1, y, z1), glm::vec3(x1, y, z0));
                }
            }
        }

        return edges;
    }
};
