#pragma once
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>
#include "grid_navmesh.hpp"
#include "../renderer/mesh/mesh.hpp"

class NavmeshMeshGenerator {
public:
    static std::shared_ptr<Mesh> Generate(
        const GridNavmesh& navmesh,
        float groundY,
        float elevationHeight)
    {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        const int width = navmesh.GetWidth();
        const int height = navmesh.GetHeight();
        const float cellSize = navmesh.GetCellSize();
        const glm::vec3 origin = navmesh.GetOrigin();
        const auto& grid = navmesh.GetGrid();

        uint32_t vertexIndex = 0;

        for (int z = 0; z < height; ++z) {
            for (int x = 0; x < width; ++x) {
                const int elevation = GetElevationSafe(grid, x, z, width, height);
                if (elevation == 0) continue;

                const float worldX0 = origin.x + static_cast<float>(x) * cellSize;
                const float worldX1 = origin.x + static_cast<float>(x + 1) * cellSize;
                const float worldZ0 = origin.z + static_cast<float>(z) * cellSize;
                const float worldZ1 = origin.z + static_cast<float>(z + 1) * cellSize;

                // Check if this is a ramp (even elevation values)
                if (elevation % 2 == 0) {
                    // Generate ramp quad with sloped corners
                    float y00, y10, y01, y11;
                    CalculateRampCorners(grid, x, z, width, height,
                                        groundY, elevationHeight, elevation,
                                        y00, y10, y01, y11);

                    // Ramp quad with varying heights at corners
                    // v0: (x0, y00, z0), v1: (x1, y10, z0)
                    // v2: (x0, y01, z1), v3: (x1, y11, z1)
                    vertices.push_back(worldX0); vertices.push_back(y00); vertices.push_back(worldZ0);
                    vertices.push_back(worldX1); vertices.push_back(y10); vertices.push_back(worldZ0);
                    vertices.push_back(worldX0); vertices.push_back(y01); vertices.push_back(worldZ1);
                    vertices.push_back(worldX1); vertices.push_back(y11); vertices.push_back(worldZ1);

                    indices.push_back(vertexIndex + 0);
                    indices.push_back(vertexIndex + 2);
                    indices.push_back(vertexIndex + 1);

                    indices.push_back(vertexIndex + 1);
                    indices.push_back(vertexIndex + 2);
                    indices.push_back(vertexIndex + 3);

                    vertexIndex += 4;

                    // Generate ramp side walls (triangular fills)
                    const int elevN = GetElevationSafe(grid, x, z - 1, width, height);
                    const int elevS = GetElevationSafe(grid, x, z + 1, width, height);
                    const int elevW = GetElevationSafe(grid, x - 1, z, width, height);
                    const int elevE = GetElevationSafe(grid, x + 1, z, width, height);

                    // Check if edges are sloped (height varies along the edge)
                    const bool westEdgeSloped = std::abs(y00 - y01) > 0.001f;
                    const bool eastEdgeSloped = std::abs(y10 - y11) > 0.001f;
                    const bool northEdgeSloped = std::abs(y00 - y10) > 0.001f;
                    const bool southEdgeSloped = std::abs(y01 - y11) > 0.001f;

                    // West side wall (x0 edge) - also generate if edge is sloped and neighbor exists
                    if (NeedsRampSideWall(elevation, elevW) || (westEdgeSloped && elevW != 0)) {
                        const float sideBaseY = GetRampSideBaseY(elevW, groundY, elevationHeight);
                        AddRampSideWall(vertices, indices, vertexIndex,
                                       worldX0, worldX0, worldZ0, worldZ1,
                                       y00, y01, sideBaseY, true);
                    }

                    // East side wall (x1 edge)
                    if (NeedsRampSideWall(elevation, elevE) || (eastEdgeSloped && elevE != 0)) {
                        const float sideBaseY = GetRampSideBaseY(elevE, groundY, elevationHeight);
                        AddRampSideWall(vertices, indices, vertexIndex,
                                       worldX1, worldX1, worldZ0, worldZ1,
                                       y10, y11, sideBaseY, false);
                    }

                    // North side wall (z0 edge)
                    if (NeedsRampSideWall(elevation, elevN) || (northEdgeSloped && elevN != 0)) {
                        const float sideBaseY = GetRampSideBaseY(elevN, groundY, elevationHeight);
                        AddRampSideWall(vertices, indices, vertexIndex,
                                       worldX0, worldX1, worldZ0, worldZ0,
                                       y00, y10, sideBaseY, false);
                    }

                    // South side wall (z1 edge)
                    if (NeedsRampSideWall(elevation, elevS) || (southEdgeSloped && elevS != 0)) {
                        const float sideBaseY = GetRampSideBaseY(elevS, groundY, elevationHeight);
                        AddRampSideWall(vertices, indices, vertexIndex,
                                       worldX0, worldX1, worldZ1, worldZ1,
                                       y01, y11, sideBaseY, true);
                    }
                } else {
                    // Floor tile (odd elevation values)
                    const float floorY = groundY + static_cast<float>((elevation - 1) / 2) * elevationHeight;

                    // Ground quad
                    vertices.push_back(worldX0); vertices.push_back(floorY); vertices.push_back(worldZ0);
                    vertices.push_back(worldX1); vertices.push_back(floorY); vertices.push_back(worldZ0);
                    vertices.push_back(worldX0); vertices.push_back(floorY); vertices.push_back(worldZ1);
                    vertices.push_back(worldX1); vertices.push_back(floorY); vertices.push_back(worldZ1);

                    indices.push_back(vertexIndex + 0);
                    indices.push_back(vertexIndex + 2);
                    indices.push_back(vertexIndex + 1);

                    indices.push_back(vertexIndex + 1);
                    indices.push_back(vertexIndex + 2);
                    indices.push_back(vertexIndex + 3);

                    vertexIndex += 4;

                    // Generate walls on edges where there's an elevation jump
                    const int elevN = GetElevationSafe(grid, x, z - 1, width, height);
                    const int elevS = GetElevationSafe(grid, x, z + 1, width, height);
                    const int elevW = GetElevationSafe(grid, x - 1, z, width, height);
                    const int elevE = GetElevationSafe(grid, x + 1, z, width, height);

                    // North wall (z - 1 direction)
                    if (NeedsWall(elevation, elevN, groundY, elevationHeight)) {
                        const float yCurrent = GetFloorY(elevation, groundY, elevationHeight);
                        const float yNeighbor = GetFloorY(elevN, groundY, elevationHeight);
                        AddWall(vertices, indices, vertexIndex,
                               worldX0, worldX1, worldZ0, worldZ0,
                               std::max(yCurrent, yNeighbor),
                               std::min(yCurrent, yNeighbor),
                               yCurrent < yNeighbor);
                    }

                    // South wall (z + 1 direction)
                    if (NeedsWall(elevation, elevS, groundY, elevationHeight)) {
                        const float yCurrent = GetFloorY(elevation, groundY, elevationHeight);
                        const float yNeighbor = GetFloorY(elevS, groundY, elevationHeight);
                        AddWall(vertices, indices, vertexIndex,
                               worldX0, worldX1, worldZ1, worldZ1,
                               std::max(yCurrent, yNeighbor),
                               std::min(yCurrent, yNeighbor),
                               yCurrent > yNeighbor);
                    }

                    // West wall (x - 1 direction)
                    if (NeedsWall(elevation, elevW, groundY, elevationHeight)) {
                        const float yCurrent = GetFloorY(elevation, groundY, elevationHeight);
                        const float yNeighbor = GetFloorY(elevW, groundY, elevationHeight);
                        AddWall(vertices, indices, vertexIndex,
                               worldX0, worldX0, worldZ0, worldZ1,
                               std::max(yCurrent, yNeighbor),
                               std::min(yCurrent, yNeighbor),
                               yCurrent > yNeighbor);
                    }

                    // East wall (x + 1 direction)
                    if (NeedsWall(elevation, elevE, groundY, elevationHeight)) {
                        const float yCurrent = GetFloorY(elevation, groundY, elevationHeight);
                        const float yNeighbor = GetFloorY(elevE, groundY, elevationHeight);
                        AddWall(vertices, indices, vertexIndex,
                               worldX1, worldX1, worldZ0, worldZ1,
                               std::max(yCurrent, yNeighbor),
                               std::min(yCurrent, yNeighbor),
                               yCurrent < yNeighbor);
                    }
                }
            }
        }

        if (!vertices.empty()) {
            return std::make_shared<Mesh>(vertices, indices, false);
        }

        return nullptr;
    }

private:
    [[nodiscard]] static int GetElevationSafe(
        const std::vector<std::vector<int>>& grid,
        int x, int z, int width, int height) noexcept
    {
        if (x < 0 || x >= width || z < 0 || z >= height) return 0;
        if (z >= static_cast<int>(grid.size())) return 0;
        if (x >= static_cast<int>(grid[z].size())) return 0;
        return grid[z][x];
    }

    [[nodiscard]] static float GetFloorY(int elevation, float groundY, float elevationHeight) noexcept
    {
        if (elevation == 0) return groundY;
        if (elevation % 2 == 1) {
            return groundY + static_cast<float>((elevation - 1) / 2) * elevationHeight;
        }
        // For ramps, use the lower floor level
        return groundY + static_cast<float>((elevation - 2) / 2) * elevationHeight;
    }

    [[nodiscard]] static bool NeedsWall(int currentElev, int neighborElev,
                                        [[maybe_unused]] float groundY,
                                        [[maybe_unused]] float elevationHeight) noexcept
    {
        // No wall if neighbor is void (0) - we only want walls between walkable areas
        if (neighborElev == 0) return false;
        if (currentElev == 0) return false;

        // Both are floors (odd numbers)
        if (currentElev % 2 == 1 && neighborElev % 2 == 1) {
            // Wall needed if elevation difference >= 2 (at least one floor level jump)
            // e.g., 1 to 3 = diff of 2, which means a jump of 1 floor level without ramp
            const int diff = std::abs(currentElev - neighborElev);
            return diff >= 2;
        }

        // Current is floor, neighbor is ramp
        if (currentElev % 2 == 1 && neighborElev % 2 == 0) {
            // Ramp connects floors at neighborElev-1 and neighborElev+1
            // Wall needed if current floor is not one of those
            const int rampLower = neighborElev - 1;
            const int rampUpper = neighborElev + 1;
            return currentElev != rampLower && currentElev != rampUpper;
        }

        // Current is ramp - no walls from ramps
        return false;
    }

    static void AddWall(std::vector<float>& vertices, std::vector<uint32_t>& indices,
                       uint32_t& vertexIndex,
                       float x0, float x1, float z0, float z1,
                       float yHigh, float yLow, bool flip) noexcept
    {
        if (yHigh <= yLow) return;

        // Wall quad from yLow to yHigh
        // v0: (x0, yLow, z0)
        // v1: (x1, yLow, z1)
        // v2: (x0, yHigh, z0)
        // v3: (x1, yHigh, z1)
        vertices.push_back(x0); vertices.push_back(yLow); vertices.push_back(z0);
        vertices.push_back(x1); vertices.push_back(yLow); vertices.push_back(z1);
        vertices.push_back(x0); vertices.push_back(yHigh); vertices.push_back(z0);
        vertices.push_back(x1); vertices.push_back(yHigh); vertices.push_back(z1);

        if (flip) {
            indices.push_back(vertexIndex + 0);
            indices.push_back(vertexIndex + 1);
            indices.push_back(vertexIndex + 2);

            indices.push_back(vertexIndex + 1);
            indices.push_back(vertexIndex + 3);
            indices.push_back(vertexIndex + 2);
        } else {
            indices.push_back(vertexIndex + 0);
            indices.push_back(vertexIndex + 2);
            indices.push_back(vertexIndex + 1);

            indices.push_back(vertexIndex + 1);
            indices.push_back(vertexIndex + 2);
            indices.push_back(vertexIndex + 3);
        }

        vertexIndex += 4;
    }

    [[nodiscard]] static bool NeedsRampSideWall(int rampElev, int neighborElev) noexcept
    {
        // Need side wall if neighbor is void
        if (neighborElev == 0) return true;

        // Need side wall if neighbor is a floor not connected to this ramp
        if (neighborElev % 2 == 1) {
            const int rampLower = rampElev - 1;
            const int rampUpper = rampElev + 1;
            // No wall if neighbor is one of the floors this ramp connects
            if (neighborElev == rampLower || neighborElev == rampUpper) return false;
            return true;
        }

        // Neighbor is also a ramp - no side wall needed
        return false;
    }

    [[nodiscard]] static float GetRampSideBaseY(int neighborElev, float groundY, float elevationHeight) noexcept
    {
        // If neighbor is void, go all the way to ground
        if (neighborElev == 0) return groundY;

        // If neighbor is a floor, go to that floor's level
        if (neighborElev % 2 == 1) {
            return groundY + static_cast<float>((neighborElev - 1) / 2) * elevationHeight;
        }

        // Neighbor is a ramp - use the lower floor it connects
        return groundY + static_cast<float>((neighborElev - 2) / 2) * elevationHeight;
    }

    static void AddRampSideWall(std::vector<float>& vertices, std::vector<uint32_t>& indices,
                                uint32_t& vertexIndex,
                                float x0, float x1, float z0, float z1,
                                float yTop0, float yTop1, float yBase, bool flip) noexcept
    {
        // Quad with sloped top edge and flat bottom edge
        // v0: (x0, yBase, z0)  - bottom corner 0
        // v1: (x1, yBase, z1)  - bottom corner 1
        // v2: (x0, yTop0, z0)  - top corner 0 (may be higher or lower than v3)
        // v3: (x1, yTop1, z1)  - top corner 1

        // Skip only if both corners match base level exactly (no gap to fill)
        if (std::abs(yTop0 - yBase) < 0.001f && std::abs(yTop1 - yBase) < 0.001f) return;

        vertices.push_back(x0); vertices.push_back(yBase); vertices.push_back(z0);
        vertices.push_back(x1); vertices.push_back(yBase); vertices.push_back(z1);
        vertices.push_back(x0); vertices.push_back(yTop0); vertices.push_back(z0);
        vertices.push_back(x1); vertices.push_back(yTop1); vertices.push_back(z1);

        if (flip) {
            indices.push_back(vertexIndex + 0);
            indices.push_back(vertexIndex + 1);
            indices.push_back(vertexIndex + 2);

            indices.push_back(vertexIndex + 1);
            indices.push_back(vertexIndex + 3);
            indices.push_back(vertexIndex + 2);
        } else {
            indices.push_back(vertexIndex + 0);
            indices.push_back(vertexIndex + 2);
            indices.push_back(vertexIndex + 1);

            indices.push_back(vertexIndex + 1);
            indices.push_back(vertexIndex + 2);
            indices.push_back(vertexIndex + 3);
        }

        vertexIndex += 4;
    }

    static void CalculateRampCorners(
        const std::vector<std::vector<int>>& grid,
        int x, int z, int width, int height,
        float groundY, float elevationHeight, int elevation,
        float& y00, float& y10, float& y01, float& y11)
    {
        const int lowerFloor = elevation - 1;
        const int upperFloor = elevation + 1;
        const float lowerY = groundY + static_cast<float>((lowerFloor - 1) / 2) * elevationHeight;
        const float upperY = groundY + static_cast<float>((upperFloor - 1) / 2) * elevationHeight;

        const int elevN = GetElevationSafe(grid, x, z - 1, width, height);
        const int elevS = GetElevationSafe(grid, x, z + 1, width, height);
        const int elevW = GetElevationSafe(grid, x - 1, z, width, height);
        const int elevE = GetElevationSafe(grid, x + 1, z, width, height);

        y00 = y10 = y01 = y11 = (lowerY + upperY) * 0.5f;

        if (elevN == lowerFloor && elevS == upperFloor) {
            y00 = y10 = lowerY;
            y01 = y11 = upperY;
        } else if (elevN == upperFloor && elevS == lowerFloor) {
            y00 = y10 = upperY;
            y01 = y11 = lowerY;
        } else if (elevW == lowerFloor && elevE == upperFloor) {
            y00 = y01 = lowerY;
            y10 = y11 = upperY;
        } else if (elevW == upperFloor && elevE == lowerFloor) {
            y00 = y01 = upperY;
            y10 = y11 = lowerY;
        }
    }
};
