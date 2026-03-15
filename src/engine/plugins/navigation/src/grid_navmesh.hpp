#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <queue>
#include <unordered_set>
#include <cmath>
#include <algorithm>

struct NavmeshRaycastHit {
    glm::vec3 Point{0.0f};
    float Distance = 0.0f;
    int CellX = 0;
    int CellZ = 0;
    int Elevation = 0;
    int RayElevation = 0;
    bool Hit = false;
};

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

    // Get elevation level of a cell (0 = not walkable, 1-3 = elevation levels)
    [[nodiscard]] int GetElevation(int x, int z) const {
        if (!IsValidCell(x, z)) return 0;
        if (z >= static_cast<int>(_grid.size())) return 0;
        if (x >= static_cast<int>(_grid[z].size())) return 0;
        return _grid[z][x];
    }

    // Check if movement between two elevation levels is allowed
    // Rules: from level N can go to N-1, N, or N+1
    [[nodiscard]] bool CanTraverse(int fromElevation, int toElevation) const {
        if (fromElevation == 0 || toElevation == 0) return false;
        const int diff = std::abs(fromElevation - toElevation);
        return diff <= 1;  // Can only move one level up or down
    }

    // Check if can move from one cell to another (walkable + elevation compatible)
    [[nodiscard]] bool CanMoveTo(int fromX, int fromZ, int toX, int toZ) const {
        if (!IsWalkableCell(fromX, fromZ) || !IsWalkableCell(toX, toZ)) return false;
        return CanTraverse(GetElevation(fromX, fromZ), GetElevation(toX, toZ));
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

    // Check if there's a clear line between two points (Bresenham) with elevation check
    [[nodiscard]] bool HasLineOfSight(const glm::vec3& from, const glm::vec3& to) const {
        int x0, z0, x1, z1;
        WorldToGrid(from, x0, z0);
        WorldToGrid(to, x1, z1);

        const int dx = std::abs(x1 - x0);
        const int dz = std::abs(z1 - z0);
        const int sx = x0 < x1 ? 1 : -1;
        const int sz = z0 < z1 ? 1 : -1;
        int err = dx - dz;

        int prevX = x0, prevZ = z0;

        while (true) {
            if (!IsWalkableCell(x0, z0)) return false;

            // Check elevation compatibility with previous cell
            if ((x0 != prevX || z0 != prevZ) && !CanMoveTo(prevX, prevZ, x0, z0)) {
                return false;
            }

            if (x0 == x1 && z0 == z1) break;

            prevX = x0;
            prevZ = z0;

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

                // Check walkable AND elevation compatibility
                if (!CanMoveTo(current.x, current.z, nx, nz) || closed[nz][nx]) continue;

                // For diagonal movement, check that both adjacent cells are traversable
                if (i % 2 == 1) {  // Diagonal
                    if (!CanMoveTo(current.x, current.z, current.x + dx[i], current.z) ||
                        !CanMoveTo(current.x, current.z, current.x, current.z + dz[i])) {
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

    // Get elevation at world position (0 = not walkable, 1-3 = elevation levels)
    [[nodiscard]] int GetElevationAt(const glm::vec3& position) const {
        int x, z;
        WorldToGrid(position, x, z);
        return GetElevation(x, z);
    }

    // Check if can move from one world position to another (considering elevation)
    [[nodiscard]] bool CanMoveBetween(const glm::vec3& from, const glm::vec3& to) const {
        int fromX, fromZ, toX, toZ;
        WorldToGrid(from, fromX, fromZ);
        WorldToGrid(to, toX, toZ);
        return CanMoveTo(fromX, fromZ, toX, toZ);
    }

    [[nodiscard]] NavmeshRaycastHit Raycast(const glm::vec3& from, const glm::vec3& to) const {
        NavmeshRaycastHit result;

        int x0, z0, x1, z1;
        WorldToGrid(from, x0, z0);
        WorldToGrid(to, x1, z1);

        // If start cell is not walkable, return immediate hit at distance 0
        if (!IsWalkableCell(x0, z0)) {
            result.Hit = true;
            result.Point = from;
            result.Distance = 0.0f;
            result.CellX = x0;
            result.CellZ = z0;
            result.Elevation = GetElevation(x0, z0);
            result.RayElevation = result.Elevation;
            return result;
        }

        int rayElevation = GetElevation(x0, z0);

        const int dx = std::abs(x1 - x0);
        const int dz = std::abs(z1 - z0);
        const int sx = x0 < x1 ? 1 : -1;
        const int sz = z0 < z1 ? 1 : -1;
        int err = dx - dz;

        while (true) {
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

            const int cellElevation = GetElevation(x0, z0);

            if (!IsWalkableCell(x0, z0) || !CanTraverse(rayElevation, cellElevation)) {
                result.Hit = true;
                result.Point = GridToWorld(x0, z0);
                result.Distance = glm::length(glm::vec2(result.Point.x - from.x, result.Point.z - from.z));
                result.CellX = x0;
                result.CellZ = z0;
                result.Elevation = cellElevation;
                result.RayElevation = rayElevation;
                return result;
            }

            rayElevation = cellElevation;
        }

        // Ray reached destination without obstruction
        result.Hit = false;
        return result;
    }

    // Calculate visual Y position with blending for transition elevations
    // Odd elevations (1, 3, 5...) are floor levels, even (2, 4, 6...) are transitions
    // elevationHeight: height per floor level (distance between odd elevations)
    [[nodiscard]] float GetVisualY(const glm::vec3& position, float groundY, float elevationHeight) const {
        int cellX, cellZ;
        WorldToGrid(position, cellX, cellZ);

        const int elevation = GetElevation(cellX, cellZ);
        if (elevation <= 0) {
            return groundY;  // Not walkable, return base
        }

        // Odd elevations are actual floor levels
        if (elevation % 2 == 1) {
            // Floor level: elevation 1 = groundY, elevation 3 = groundY + elevationHeight, etc.
            const int floorLevel = (elevation - 1) / 2;
            return groundY + static_cast<float>(floorLevel) * elevationHeight;
        }

        // Even elevations are transitions - blend based on position within cell
        const int lowerFloor = elevation - 1;  // Odd number below (e.g., 2 -> 1)
        const int upperFloor = elevation + 1;  // Odd number above (e.g., 2 -> 3)

        const float lowerY = groundY + static_cast<float>((lowerFloor - 1) / 2) * elevationHeight;
        const float upperY = groundY + static_cast<float>((upperFloor - 1) / 2) * elevationHeight;

        // Find ramp direction by checking neighbors
        // Look for neighbors with lower and upper elevations
        int lowerDirX = 0, lowerDirZ = 0;
        int upperDirX = 0, upperDirZ = 0;

        const int dx[] = {0, 1, 0, -1};  // N, E, S, W
        const int dz[] = {-1, 0, 1, 0};

        for (int i = 0; i < 4; ++i) {
            const int nx = cellX + dx[i];
            const int nz = cellZ + dz[i];
            const int neighborElev = GetElevation(nx, nz);

            if (neighborElev == lowerFloor) {
                lowerDirX = dx[i];
                lowerDirZ = dz[i];
            } else if (neighborElev == upperFloor) {
                upperDirX = dx[i];
                upperDirZ = dz[i];
            }
        }

        // Calculate position within cell (0 to 1)
        const float cellMinX = _origin.x + static_cast<float>(cellX) * _cellSize;
        const float cellMinZ = _origin.z + static_cast<float>(cellZ) * _cellSize;
        const float localX = (position.x - cellMinX) / _cellSize;  // 0 to 1
        const float localZ = (position.z - cellMinZ) / _cellSize;  // 0 to 1

        // Calculate blend factor based on ramp direction
        float t = 0.5f;  // Default to middle if direction unclear

        if (lowerDirX != 0 || upperDirX != 0) {
            // Ramp along X axis
            if (lowerDirX < 0 || upperDirX > 0) {
                // Lower on -X (west), upper on +X (east)
                t = localX;
            } else {
                // Lower on +X (east), upper on -X (west)
                t = 1.0f - localX;
            }
        } else if (lowerDirZ != 0 || upperDirZ != 0) {
            // Ramp along Z axis
            if (lowerDirZ < 0 || upperDirZ > 0) {
                // Lower on -Z (north), upper on +Z (south)
                t = localZ;
            } else {
                // Lower on +Z (south), upper on -Z (north)
                t = 1.0f - localZ;
            }
        }

        // Lerp between lower and upper Y
        return lowerY + t * (upperY - lowerY);
    }

    // Getters
    [[nodiscard]] int GetWidth() const noexcept { return _width; }
    [[nodiscard]] int GetHeight() const noexcept { return _height; }
    [[nodiscard]] float GetCellSize() const noexcept { return _cellSize; }
    [[nodiscard]] const glm::vec3& GetOrigin() const noexcept { return _origin; }
    [[nodiscard]] const std::vector<std::vector<int>>& GetGrid() const noexcept { return _grid; }

    // Get boundary edges for RVO2 obstacles (each edge is a line segment)
    // Includes edges at non-walkable boundaries AND steep elevation changes
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

                // Add edge if neighbor is non-walkable OR has incompatible elevation (CCW winding for RVO2)
                // North edge (z-1)
                if (!CanMoveTo(x, z, x, z - 1)) {
                    edges.emplace_back(glm::vec3(x1, y, z0), glm::vec3(x0, y, z0));
                }
                // South edge (z+1)
                if (!CanMoveTo(x, z, x, z + 1)) {
                    edges.emplace_back(glm::vec3(x0, y, z1), glm::vec3(x1, y, z1));
                }
                // West edge (x-1)
                if (!CanMoveTo(x, z, x - 1, z)) {
                    edges.emplace_back(glm::vec3(x0, y, z0), glm::vec3(x0, y, z1));
                }
                // East edge (x+1)
                if (!CanMoveTo(x, z, x + 1, z)) {
                    edges.emplace_back(glm::vec3(x1, y, z1), glm::vec3(x1, y, z0));
                }
            }
        }

        return edges;
    }
};
