#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class dtNavMesh;
class dtNavMeshQuery;

class DetourNavmesh {
private:
    dtNavMesh* _navMesh = nullptr;
    dtNavMeshQuery* _navQuery = nullptr;

    int _width = 0;
    int _height = 0;
    float _cellSize = 1.0f;
    glm::vec3 _origin = glm::vec3(0.0f);

public:
    DetourNavmesh() = default;
    ~DetourNavmesh();

    // Delete copy constructor and assignment operator
    DetourNavmesh(const DetourNavmesh&) = delete;
    DetourNavmesh& operator=(const DetourNavmesh&) = delete;

    // Initialize navmesh from walkability grid
    bool Initialize(int width, int height, float cellSize, const glm::vec3& origin,
                   const std::vector<std::vector<int>>& walkabilityGrid);

    // Pathfinding
    std::vector<glm::vec3> FindPath(const glm::vec3& start, const glm::vec3& end);

    // Query functions
    glm::vec3 FindNearestPoint(const glm::vec3& position) const;
    bool IsWalkable(const glm::vec3& position) const;

    // Getters
    dtNavMesh* GetNavMesh() const noexcept { return _navMesh; }
    dtNavMeshQuery* GetNavQuery() const noexcept { return _navQuery; }
    int GetWidth() const noexcept { return _width; }
    int GetHeight() const noexcept { return _height; }
    float GetCellSize() const noexcept { return _cellSize; }
    const glm::vec3& GetOrigin() const noexcept { return _origin; }
};
