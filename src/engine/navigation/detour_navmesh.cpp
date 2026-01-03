#include "detour_navmesh.hpp"
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <Recast.h>
#include <cstring>
#include <algorithm>
#include <iostream>

DetourNavmesh::~DetourNavmesh() {
    if (_navQuery) {
        dtFreeNavMeshQuery(_navQuery);
        _navQuery = nullptr;
    }
    if (_navMesh) {
        dtFreeNavMesh(_navMesh);
        _navMesh = nullptr;
    }
}

bool DetourNavmesh::Initialize(int width, int height, float cellSize, const glm::vec3& origin,
                               const std::vector<std::vector<int>>& walkabilityGrid) {
    _width = width;
    _height = height;
    _cellSize = cellSize;
    _origin = origin;

    // Build navmesh with SHARED vertices between adjacent polygons
    // This is critical for Detour's pathfinding to work correctly

    const int nvp = 6;  // Max verts per polygon

    // Create vertex grid - vertices are shared at grid intersections
    // Vertex at grid position (x, z) has index: z * (width+1) + x
    std::vector<unsigned short> vertices;
    vertices.reserve((width + 1) * (height + 1) * 3);

    for (int z = 0; z <= height; ++z) {
        for (int x = 0; x <= width; ++x) {
            vertices.push_back(static_cast<unsigned short>(x));
            vertices.push_back(static_cast<unsigned short>(1));  // Y = 1 (flat)
            vertices.push_back(static_cast<unsigned short>(z));
        }
    }

    // Build polygon index grid
    std::vector<std::vector<int>> polyIndexGrid(height, std::vector<int>(width, -1));
    int polyCount = 0;

    // Count walkable cells
    for (int z = 0; z < height && z < static_cast<int>(walkabilityGrid.size()); ++z) {
        const auto& row = walkabilityGrid[z];
        for (int x = 0; x < width && x < static_cast<int>(row.size()); ++x) {
            if (row[x] != 0) {
                polyIndexGrid[z][x] = polyCount++;
            }
        }
    }

    if (polyCount == 0) {
        return false;
    }

    // Build polygons with shared vertex references
    std::vector<unsigned short> polygons;
    std::vector<unsigned short> polyFlags;
    std::vector<unsigned char> polyAreas;

    polygons.reserve(polyCount * nvp * 2);
    polyFlags.reserve(polyCount);
    polyAreas.reserve(polyCount);

    for (int z = 0; z < height && z < static_cast<int>(walkabilityGrid.size()); ++z) {
        const auto& row = walkabilityGrid[z];
        for (int x = 0; x < width && x < static_cast<int>(row.size()); ++x) {
            if (row[x] == 0) continue;  // Not walkable

            // Vertex indices for this cell's corners (shared with neighbors)
            // Vertex at (x, z) = z * (width+1) + x
            const unsigned short v0 = static_cast<unsigned short>(z * (width + 1) + x);         // bottom-left
            const unsigned short v1 = static_cast<unsigned short>(z * (width + 1) + x + 1);     // bottom-right
            const unsigned short v2 = static_cast<unsigned short>((z + 1) * (width + 1) + x + 1); // top-right
            const unsigned short v3 = static_cast<unsigned short>((z + 1) * (width + 1) + x);   // top-left

            // Add polygon vertices (CLOCKWISE for Detour)
            polygons.push_back(v0);
            polygons.push_back(v3);
            polygons.push_back(v2);
            polygons.push_back(v1);
            polygons.push_back(0xffff);  // unused
            polygons.push_back(0xffff);  // unused

            // Add neighbor connections (matching new vertex order)
            // Edge 0: v0->v3 (left edge, -X neighbor)
            if (x > 0 && polyIndexGrid[z][x - 1] != -1) {
                polygons.push_back(static_cast<unsigned short>(polyIndexGrid[z][x - 1]));
            } else {
                polygons.push_back(0xffff);
            }

            // Edge 1: v3->v2 (top edge, +Z neighbor)
            if (z + 1 < height && polyIndexGrid[z + 1][x] != -1) {
                polygons.push_back(static_cast<unsigned short>(polyIndexGrid[z + 1][x]));
            } else {
                polygons.push_back(0xffff);
            }

            // Edge 2: v2->v1 (right edge, +X neighbor)
            if (x + 1 < width && polyIndexGrid[z][x + 1] != -1) {
                polygons.push_back(static_cast<unsigned short>(polyIndexGrid[z][x + 1]));
            } else {
                polygons.push_back(0xffff);
            }

            // Edge 3: v1->v0 (bottom edge, -Z neighbor)
            if (z > 0 && polyIndexGrid[z - 1][x] != -1) {
                polygons.push_back(static_cast<unsigned short>(polyIndexGrid[z - 1][x]));
            } else {
                polygons.push_back(0xffff);
            }

            // Fill remaining neighbor slots
            polygons.push_back(0xffff);
            polygons.push_back(0xffff);

            polyFlags.push_back(1);  // Walkable
            polyAreas.push_back(RC_WALKABLE_AREA);
        }
    }

    // Setup navmesh creation parameters
    dtNavMeshCreateParams params;
    memset(&params, 0, sizeof(params));
    params.verts = vertices.data();
    params.vertCount = static_cast<int>(vertices.size() / 3);
    params.polys = polygons.data();
    params.polyAreas = polyAreas.data();
    params.polyFlags = polyFlags.data();
    params.polyCount = polyCount;
    params.nvp = nvp;
    params.walkableHeight = 2.0f;
    params.walkableRadius = 0.6f;
    params.walkableClimb = 0.9f;
    params.bmin[0] = origin.x;
    params.bmin[1] = origin.y - 1.0f;
    params.bmin[2] = origin.z;
    params.bmax[0] = origin.x + width * cellSize;
    params.bmax[1] = origin.y + 1.0f;
    params.bmax[2] = origin.z + height * cellSize;
    params.cs = cellSize;
    params.ch = cellSize;
    params.buildBvTree = true;

    unsigned char* navData = nullptr;
    int navDataSize = 0;

    if (!dtCreateNavMeshData(&params, &navData, &navDataSize)) {
        return false;
    }

    // Create navmesh
    _navMesh = dtAllocNavMesh();
    if (!_navMesh) {
        dtFree(navData);
        return false;
    }

    dtStatus status = _navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
    if (dtStatusFailed(status)) {
        dtFreeNavMesh(_navMesh);
        _navMesh = nullptr;
        return false;
    }

    // Create navmesh query
    _navQuery = dtAllocNavMeshQuery();
    if (!_navQuery) {
        dtFreeNavMesh(_navMesh);
        _navMesh = nullptr;
        return false;
    }

    status = _navQuery->init(_navMesh, 2048);
    if (dtStatusFailed(status)) {
        dtFreeNavMeshQuery(_navQuery);
        _navQuery = nullptr;
        dtFreeNavMesh(_navMesh);
        _navMesh = nullptr;
        return false;
    }

    return true;
}

std::vector<glm::vec3> DetourNavmesh::FindPath(const glm::vec3& start, const glm::vec3& end) {
    if (!_navQuery || !_navMesh) {
        return {};
    }

    const float extents[3] = { 2.0f, 4.0f, 2.0f };
    dtQueryFilter filter;
    filter.setIncludeFlags(1);
    filter.setExcludeFlags(0);

    // Find start and end polygons
    dtPolyRef startRef = 0;
    float startPos[3] = { start.x, start.y, start.z };
    float nearestStartPos[3];
    _navQuery->findNearestPoly(startPos, extents, &filter, &startRef, nearestStartPos);

    dtPolyRef endRef = 0;
    float endPos[3] = { end.x, end.y, end.z };
    float nearestEndPos[3];
    _navQuery->findNearestPoly(endPos, extents, &filter, &endRef, nearestEndPos);

    if (!startRef || !endRef) {
        return {};
    }

    // Find path using the snapped positions for polygon corridor
    constexpr int MAX_POLYS = 256;
    dtPolyRef path[MAX_POLYS];
    int pathCount = 0;

    _navQuery->findPath(startRef, endRef, nearestStartPos, nearestEndPos, &filter, path, &pathCount, MAX_POLYS);

    if (pathCount == 0) {
        return {};
    }

    // Get straight path using snapped positions
    constexpr int MAX_STRAIGHT_PATH = 256;
    float straightPath[MAX_STRAIGHT_PATH * 3];
    unsigned char straightPathFlags[MAX_STRAIGHT_PATH];
    dtPolyRef straightPathRefs[MAX_STRAIGHT_PATH];
    int straightPathCount = 0;

    _navQuery->findStraightPath(nearestStartPos, nearestEndPos, path, pathCount,
                               straightPath, straightPathFlags, straightPathRefs,
                               &straightPathCount, MAX_STRAIGHT_PATH);

    // Convert to glm::vec3, but use original start position as first point
    std::vector<glm::vec3> result;
    result.reserve(straightPathCount + 1);

    // Always start from the actual start position
    result.push_back(start);

    // Add path waypoints (skip first if it's very close to start)
    for (int i = 0; i < straightPathCount; ++i) {
        glm::vec3 waypoint(straightPath[i * 3], straightPath[i * 3 + 1], straightPath[i * 3 + 2]);

        // Skip if too close to previous point
        if (!result.empty()) {
            float dist = glm::length(glm::vec2(waypoint.x - result.back().x, waypoint.z - result.back().z));
            if (dist < 0.3f) {
                continue;
            }
        }
        result.push_back(waypoint);
    }

    return result;
}

glm::vec3 DetourNavmesh::FindNearestPoint(const glm::vec3& position) const {
    if (!_navQuery || !_navMesh) {
        return position;
    }

    const float extents[3] = { 2.0f, 4.0f, 2.0f };
    dtQueryFilter filter;
    filter.setIncludeFlags(1);
    filter.setExcludeFlags(0);

    dtPolyRef polyRef = 0;
    float nearestPt[3];
    float pos[3] = { position.x, position.y, position.z };

    _navQuery->findNearestPoly(pos, extents, &filter, &polyRef, nearestPt);

    if (polyRef) {
        return glm::vec3(nearestPt[0], nearestPt[1], nearestPt[2]);
    }

    return position;
}

bool DetourNavmesh::IsWalkable(const glm::vec3& position) const {
    if (!_navQuery || !_navMesh) {
        return false;
    }

    const float extents[3] = { 0.1f, 1.0f, 0.1f };
    dtQueryFilter filter;
    filter.setIncludeFlags(1);
    filter.setExcludeFlags(0);

    dtPolyRef polyRef = 0;
    float pos[3] = { position.x, position.y, position.z };

    _navQuery->findNearestPoly(pos, extents, &filter, &polyRef, nullptr);

    return polyRef != 0;
}
