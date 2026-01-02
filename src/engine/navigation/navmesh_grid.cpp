#include "navmesh_grid.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

NavmeshGrid::NavmeshGrid(int width, int height, float cellSize, const glm::vec3& origin)
    : _width(width), _height(height), _cellSize(cellSize), _origin(origin) {

    _cells.resize(width * height);

    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            const int idx = GridToIndex(x, z);
            _cells[idx].X = x;
            _cells[idx].Z = z;
            _cells[idx].Walkable = true;
            _cells[idx].Cost = 1.0f;
        }
    }
}

glm::ivec2 NavmeshGrid::WorldToGrid(const glm::vec3& worldPos) const noexcept {
    const glm::vec3 localPos = worldPos - _origin;
    return glm::ivec2(
        static_cast<int>(std::floor(localPos.x / _cellSize)),
        static_cast<int>(std::floor(localPos.z / _cellSize))
    );
}

glm::vec3 NavmeshGrid::GridToWorld(const glm::ivec2& gridPos) const noexcept {
    return GridToWorld(gridPos.x, gridPos.y);
}

glm::vec3 NavmeshGrid::GridToWorld(int x, int z) const noexcept {
    return _origin + glm::vec3(
        (static_cast<float>(x) + 0.5f) * _cellSize,
        0.0f,
        (static_cast<float>(z) + 0.5f) * _cellSize
    );
}

int NavmeshGrid::GridToIndex(int x, int z) const noexcept {
    return z * _width + x;
}

glm::ivec2 NavmeshGrid::IndexToGrid(int index) const noexcept {
    const int z = index / _width;
    const int x = index % _width;
    return glm::ivec2(x, z);
}

bool NavmeshGrid::IsValidCell(int x, int z) const noexcept {
    return x >= 0 && x < _width && z >= 0 && z < _height;
}

bool NavmeshGrid::IsWalkable(int x, int z) const noexcept {
    if (!IsValidCell(x, z)) return false;
    return _cells[GridToIndex(x, z)].Walkable;
}

void NavmeshGrid::SetCellWalkable(int x, int z, bool walkable) noexcept {
    if (IsValidCell(x, z)) {
        _cells[GridToIndex(x, z)].Walkable = walkable;
    }
}

void NavmeshGrid::SetCellCost(int x, int z, float cost) noexcept {
    if (IsValidCell(x, z)) {
        _cells[GridToIndex(x, z)].Cost = cost;
    }
}

std::vector<int> NavmeshGrid::GetNeighbors(int index) const {
    std::vector<int> neighbors;
    neighbors.reserve(8);

    const glm::ivec2 gridPos = IndexToGrid(index);
    const int x = gridPos.x;
    const int z = gridPos.y;

    for (int dz = -1; dz <= 1; ++dz) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dz == 0) continue;

            const int nx = x + dx;
            const int nz = z + dz;

            if (IsWalkable(nx, nz)) {
                neighbors.push_back(GridToIndex(nx, nz));
            }
        }
    }

    std::sort(neighbors.begin(), neighbors.end());
    return neighbors;
}

float NavmeshGrid::ManhattanDistance(int x1, int z1, int x2, int z2) const noexcept {
    return static_cast<float>(std::abs(x1 - x2) + std::abs(z1 - z2));
}

std::vector<glm::vec3> NavmeshGrid::ReconstructPath(const std::unordered_map<int, AStarNode>& nodes, int endIdx) const {
    std::vector<glm::vec3> path;
    int idx = endIdx;

    while (idx != -1) {
        const NavCell& cell = _cells[idx];
        path.push_back(GridToWorld(cell.X, cell.Z));

        const auto it = nodes.find(idx);
        if (it == nodes.end()) break;
        idx = it->second.ParentIndex;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<glm::vec3> NavmeshGrid::FindPath(const glm::vec3& start, const glm::vec3& end) {
    const glm::ivec2 startGrid = WorldToGrid(start);
    const glm::ivec2 endGrid = WorldToGrid(end);

    if (!IsWalkable(startGrid.x, startGrid.y)) {
        return {};
    }

    if (!IsWalkable(endGrid.x, endGrid.y)) {
        return {};
    }

    const int startIdx = GridToIndex(startGrid.x, startGrid.y);
    const int endIdx = GridToIndex(endGrid.x, endGrid.y);

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
        ManhattanDistance(startGrid.x, startGrid.y, endGrid.x, endGrid.y)
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
            const glm::ivec2 neighborGrid = IndexToGrid(neighborIdx);
            const glm::ivec2 currentGrid = IndexToGrid(current.Index);

            const float movementCost = glm::length(glm::vec2(
                static_cast<float>(neighborGrid.x - currentGrid.x),
                static_cast<float>(neighborGrid.y - currentGrid.y)
            ));

            const float tentativeG = current.GCost + movementCost * neighborCell.Cost;

            if (!nodes.count(neighborIdx) || tentativeG < nodes[neighborIdx].GCost) {
                const float hCost = ManhattanDistance(
                    neighborCell.X, neighborCell.Z,
                    endGrid.x, endGrid.y
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
    const glm::ivec2 fromGrid = WorldToGrid(from);
    const glm::ivec2 toGrid = WorldToGrid(to);

    if (!IsWalkable(fromGrid.x, fromGrid.y)) return false;
    if (!IsWalkable(toGrid.x, toGrid.y)) return false;

    const glm::vec3 direction = to - from;
    const float distance = glm::length(direction);

    if (distance < 0.0001f) return true;

    const glm::vec3 step = direction / distance;
    constexpr float checkInterval = 0.5f;
    const int steps = static_cast<int>(distance / checkInterval);

    for (int i = 1; i < steps; ++i) {
        const glm::vec3 checkPos = from + step * (static_cast<float>(i) * checkInterval);
        const glm::ivec2 checkGrid = WorldToGrid(checkPos);

        if (!IsWalkable(checkGrid.x, checkGrid.y)) {
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

        const glm::ivec2 minGrid = WorldToGrid(minBound);
        const glm::ivec2 maxGrid = WorldToGrid(maxBound);

        for (int z = std::max(0, minGrid.y); z <= std::min(_height - 1, maxGrid.y); ++z) {
            for (int x = std::max(0, minGrid.x); x <= std::min(_width - 1, maxGrid.x); ++x) {
                SetCellWalkable(x, z, true);
            }
        }
    }
}
