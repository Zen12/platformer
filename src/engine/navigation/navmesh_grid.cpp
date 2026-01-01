#include "navmesh_grid.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

NavmeshGrid::NavmeshGrid(int width, int height, int depth, float cellSize, const glm::vec3& origin)
    : _width(width), _height(height), _depth(depth), _cellSize(cellSize), _origin(origin) {

    _cells.resize(width * height * depth);

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const int idx = GridToIndex(x, y, z);
                _cells[idx].X = x;
                _cells[idx].Y = y;
                _cells[idx].Z = z;
                _cells[idx].Walkable = true;
                _cells[idx].Cost = 1.0f;
            }
        }
    }
}

glm::ivec3 NavmeshGrid::WorldToGrid(const glm::vec3& worldPos) const noexcept {
    const glm::vec3 localPos = worldPos - _origin;
    return glm::ivec3(
        static_cast<int>(std::floor(localPos.x / _cellSize)),
        static_cast<int>(std::floor(localPos.y / _cellSize)),
        static_cast<int>(std::floor(localPos.z / _cellSize))
    );
}

glm::vec3 NavmeshGrid::GridToWorld(const glm::ivec3& gridPos) const noexcept {
    return GridToWorld(gridPos.x, gridPos.y, gridPos.z);
}

glm::vec3 NavmeshGrid::GridToWorld(int x, int y, int z) const noexcept {
    return _origin + glm::vec3(
        (static_cast<float>(x) + 0.5f) * _cellSize,
        (static_cast<float>(y) + 0.5f) * _cellSize,
        (static_cast<float>(z) + 0.5f) * _cellSize
    );
}

int NavmeshGrid::GridToIndex(int x, int y, int z) const noexcept {
    return z * (_width * _height) + y * _width + x;
}

glm::ivec3 NavmeshGrid::IndexToGrid(int index) const noexcept {
    const int z = index / (_width * _height);
    const int remainder = index % (_width * _height);
    const int y = remainder / _width;
    const int x = remainder % _width;
    return glm::ivec3(x, y, z);
}

bool NavmeshGrid::IsValidCell(int x, int y, int z) const noexcept {
    return x >= 0 && x < _width && y >= 0 && y < _height && z >= 0 && z < _depth;
}

bool NavmeshGrid::IsWalkable(int x, int y, int z) const noexcept {
    if (!IsValidCell(x, y, z)) return false;
    return _cells[GridToIndex(x, y, z)].Walkable;
}

void NavmeshGrid::SetCellWalkable(int x, int y, int z, bool walkable) noexcept {
    if (IsValidCell(x, y, z)) {
        _cells[GridToIndex(x, y, z)].Walkable = walkable;
    }
}

void NavmeshGrid::SetCellCost(int x, int y, int z, float cost) noexcept {
    if (IsValidCell(x, y, z)) {
        _cells[GridToIndex(x, y, z)].Cost = cost;
    }
}

std::vector<int> NavmeshGrid::GetNeighbors(int index) const {
    std::vector<int> neighbors;
    neighbors.reserve(26);

    const glm::ivec3 gridPos = IndexToGrid(index);
    const int x = gridPos.x;
    const int y = gridPos.y;
    const int z = gridPos.z;

    for (int dz = -1; dz <= 1; ++dz) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0 && dz == 0) continue;

                const int nx = x + dx;
                const int ny = y + dy;
                const int nz = z + dz;

                if (IsWalkable(nx, ny, nz)) {
                    neighbors.push_back(GridToIndex(nx, ny, nz));
                }
            }
        }
    }

    std::sort(neighbors.begin(), neighbors.end());
    return neighbors;
}

float NavmeshGrid::ManhattanDistance(int x1, int y1, int z1, int x2, int y2, int z2) const noexcept {
    return static_cast<float>(std::abs(x1 - x2) + std::abs(y1 - y2) + std::abs(z1 - z2));
}

std::vector<glm::vec3> NavmeshGrid::ReconstructPath(const std::unordered_map<int, AStarNode>& nodes, int endIdx) const {
    std::vector<glm::vec3> path;
    int idx = endIdx;

    while (idx != -1) {
        const NavCell& cell = _cells[idx];
        path.push_back(GridToWorld(cell.X, cell.Y, cell.Z));

        const auto it = nodes.find(idx);
        if (it == nodes.end()) break;
        idx = it->second.ParentIndex;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<glm::vec3> NavmeshGrid::FindPath(const glm::vec3& start, const glm::vec3& end) {
    const glm::ivec3 startGrid = WorldToGrid(start);
    const glm::ivec3 endGrid = WorldToGrid(end);

    if (!IsWalkable(startGrid.x, startGrid.y, startGrid.z)) {
        return {};
    }

    if (!IsWalkable(endGrid.x, endGrid.y, endGrid.z)) {
        return {};
    }

    const int startIdx = GridToIndex(startGrid.x, startGrid.y, startGrid.z);
    const int endIdx = GridToIndex(endGrid.x, endGrid.y, endGrid.z);

    if (startIdx == endIdx) {
        return {start, end};
    }

    auto cmp = [](const AStarNode& a, const AStarNode& b) {
        constexpr float EPSILON = 0.0001f;
        if (std::abs(a.FCost() - b.FCost()) < EPSILON) {
            return a.Index > b.Index;
        }
        return a.FCost() > b.FCost();
    };

    std::priority_queue<AStarNode, std::vector<AStarNode>, decltype(cmp)> openSet(cmp);
    std::unordered_set<int> closedSet;
    std::unordered_map<int, AStarNode> nodes;

    AStarNode startNode{
        startIdx,
        -1,
        0.0f,
        ManhattanDistance(startGrid.x, startGrid.y, startGrid.z, endGrid.x, endGrid.y, endGrid.z)
    };

    openSet.push(startNode);
    nodes[startIdx] = startNode;

    while (!openSet.empty()) {
        const AStarNode current = openSet.top();
        openSet.pop();

        if (current.Index == endIdx) {
            std::vector<glm::vec3> path = ReconstructPath(nodes, endIdx);
            return SmoothPath(path);
        }

        if (closedSet.count(current.Index)) {
            continue;
        }

        closedSet.insert(current.Index);

        const std::vector<int> neighbors = GetNeighbors(current.Index);
        for (const int neighborIdx : neighbors) {
            if (closedSet.count(neighborIdx)) continue;

            const NavCell& neighborCell = _cells[neighborIdx];
            const glm::ivec3 neighborGrid = IndexToGrid(neighborIdx);
            const glm::ivec3 currentGrid = IndexToGrid(current.Index);

            const float movementCost = glm::length(glm::vec3(
                static_cast<float>(neighborGrid.x - currentGrid.x),
                static_cast<float>(neighborGrid.y - currentGrid.y),
                static_cast<float>(neighborGrid.z - currentGrid.z)
            ));

            const float tentativeG = current.GCost + movementCost * neighborCell.Cost;

            if (!nodes.count(neighborIdx) || tentativeG < nodes[neighborIdx].GCost) {
                const float hCost = ManhattanDistance(
                    neighborCell.X, neighborCell.Y, neighborCell.Z,
                    endGrid.x, endGrid.y, endGrid.z
                );

                AStarNode neighborNode{
                    neighborIdx,
                    current.Index,
                    tentativeG,
                    hCost
                };

                nodes[neighborIdx] = neighborNode;
                openSet.push(neighborNode);
            }
        }
    }

    return {};
}

bool NavmeshGrid::HasLineOfSight(const glm::vec3& from, const glm::vec3& to) const noexcept {
    const glm::ivec3 fromGrid = WorldToGrid(from);
    const glm::ivec3 toGrid = WorldToGrid(to);

    if (!IsWalkable(fromGrid.x, fromGrid.y, fromGrid.z)) return false;
    if (!IsWalkable(toGrid.x, toGrid.y, toGrid.z)) return false;

    const glm::vec3 direction = to - from;
    const float distance = glm::length(direction);

    if (distance < 0.0001f) return true;

    const glm::vec3 step = direction / distance;
    constexpr float checkInterval = 0.5f;
    const int steps = static_cast<int>(distance / checkInterval);

    for (int i = 1; i < steps; ++i) {
        const glm::vec3 checkPos = from + step * (static_cast<float>(i) * checkInterval);
        const glm::ivec3 checkGrid = WorldToGrid(checkPos);

        if (!IsWalkable(checkGrid.x, checkGrid.y, checkGrid.z)) {
            return false;
        }
    }

    return true;
}

std::vector<glm::vec3> NavmeshGrid::SmoothPath(const std::vector<glm::vec3>& path) {
    if (path.size() <= 2) return path;

    std::vector<glm::vec3> smoothed;
    smoothed.push_back(path[0]);

    size_t i = 0;
    while (i < path.size() - 1) {
        size_t farthest = i + 1;

        for (size_t j = i + 2; j < path.size(); ++j) {
            if (HasLineOfSight(path[i], path[j])) {
                farthest = j;
            }
        }

        smoothed.push_back(path[farthest]);
        i = farthest;
    }

    return smoothed;
}

void NavmeshGrid::BakeFromGeometry(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices) {
    for (auto& cell : _cells) {
        cell.Walkable = false;
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        const glm::vec3& v0 = vertices[indices[i]];
        const glm::vec3& v1 = vertices[indices[i + 1]];
        const glm::vec3& v2 = vertices[indices[i + 2]];

        const glm::vec3 minBound = glm::min(glm::min(v0, v1), v2);
        const glm::vec3 maxBound = glm::max(glm::max(v0, v1), v2);

        const glm::ivec3 minGrid = WorldToGrid(minBound);
        const glm::ivec3 maxGrid = WorldToGrid(maxBound);

        for (int z = std::max(0, minGrid.z); z <= std::min(_depth - 1, maxGrid.z); ++z) {
            for (int y = std::max(0, minGrid.y); y <= std::min(_height - 1, maxGrid.y); ++y) {
                for (int x = std::max(0, minGrid.x); x <= std::min(_width - 1, maxGrid.x); ++x) {
                    SetCellWalkable(x, y, z, true);
                }
            }
        }
    }
}
