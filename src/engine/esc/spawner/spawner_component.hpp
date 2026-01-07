#pragma once
#include <string>
#include <utility>
#include <vector>
#include <glm/glm.hpp>

struct SpawnerComponent {
private:
    std::string _prefabGuid;
    int _spawnCount;
    std::vector<glm::vec3> _spawnPositions;
    bool _spawnOnNavmesh;
    bool _spawnOnAllCells;
    float _elevationHeight;
    float _yOffset;
    bool _hasSpawned;

public:
    SpawnerComponent(std::string  prefabGuid, const int &spawnCount, const std::vector<glm::vec3>& spawnPositions, bool spawnOnNavmesh, bool spawnOnAllCells, float elevationHeight = 1.0f, float yOffset = 0.0f)
        : _prefabGuid(std::move(prefabGuid)), _spawnCount(spawnCount), _spawnPositions(spawnPositions), _spawnOnNavmesh(spawnOnNavmesh), _spawnOnAllCells(spawnOnAllCells), _elevationHeight(elevationHeight), _yOffset(yOffset), _hasSpawned(false) {}

    [[nodiscard]] std::string GetPrefabGuid() const noexcept { return _prefabGuid; }
    [[nodiscard]] int GetSpawnCount() const noexcept { return _spawnCount; }
    [[nodiscard]] std::vector<glm::vec3> GetSpawnPositions() const noexcept { return _spawnPositions; }
    [[nodiscard]] bool GetSpawnOnNavmesh() const noexcept { return _spawnOnNavmesh; }
    [[nodiscard]] bool GetSpawnOnAllCells() const noexcept { return _spawnOnAllCells; }
    [[nodiscard]] float GetElevationHeight() const noexcept { return _elevationHeight; }
    [[nodiscard]] float GetYOffset() const noexcept { return _yOffset; }
    [[nodiscard]] bool HasSpawned() const noexcept { return _hasSpawned; }

    void SetHasSpawned(bool spawned) noexcept { _hasSpawned = spawned; }
};
