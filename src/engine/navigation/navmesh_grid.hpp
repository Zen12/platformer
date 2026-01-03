#pragma once
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include <unordered_map>
#include <queue>
#include <unordered_set>

struct NavCell {
    bool Walkable = true;
    float Cost = 1.0f;
    int X = 0;
    int Z = 0;
};

class NavmeshGrid {
private:
    std::vector<NavCell> _cells;
    int _width, _height;
    float _cellSize;
    glm::vec3 _origin;

    struct AStarNode {
        int Index;
        int ParentIndex;
        float GCost;
        float HCost;

        float FCost() const { return GCost + HCost; }
    };

    [[nodiscard]] float ManhattanDistance(int x1, int z1, int x2, int z2) const noexcept;
    [[nodiscard]] bool HasLineOfSight(const glm::vec3& from, const glm::vec3& to) const noexcept;
    std::vector<glm::vec3> ReconstructPath(const std::unordered_map<int, AStarNode>& nodes, int endIdx) const;

public:
    NavmeshGrid(int width, int height, float cellSize, const glm::vec3& origin);

    [[nodiscard]] std::vector<glm::vec3> FindPath(const glm::vec3& start, const glm::vec3& end);
    [[nodiscard]] std::vector<glm::vec3> SmoothPath(const std::vector<glm::vec3>& path);

    [[nodiscard]] glm::ivec2 WorldToGrid(const glm::vec3& worldPos) const noexcept;
    [[nodiscard]] glm::vec3 GridToWorld(const glm::ivec2& gridPos) const noexcept;
    [[nodiscard]] glm::vec3 GridToWorld(int x, int z) const noexcept;
    [[nodiscard]] int GridToIndex(int x, int z) const noexcept;
    [[nodiscard]] glm::ivec2 IndexToGrid(int index) const noexcept;

    [[nodiscard]] bool IsWalkable(int x, int z) const noexcept;
    [[nodiscard]] bool IsValidCell(int x, int z) const noexcept;
    [[nodiscard]] std::vector<int> GetNeighbors(int index) const;

    [[nodiscard]] glm::vec3 FindClosestWalkablePoint(const glm::vec3& worldPos) const noexcept;

    void SetCellWalkable(int x, int z, bool walkable) noexcept;
    void SetCellCost(int x, int z, float cost) noexcept;

    void BakeFromGeometry(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices);

    [[nodiscard]] int GetWidth() const noexcept { return _width; }
    [[nodiscard]] int GetHeight() const noexcept { return _height; }
    [[nodiscard]] float GetCellSize() const noexcept { return _cellSize; }
    [[nodiscard]] glm::vec3 GetOrigin() const noexcept { return _origin; }
};
