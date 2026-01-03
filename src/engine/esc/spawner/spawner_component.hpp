#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct SpawnerComponent {
private:
    std::string _prefabGuid;
    int _spawnCount;
    std::vector<glm::vec3> _spawnPositions;
    bool _spawnOnNavmesh;
    bool _spawnOnAllCells;
    bool _hasSpawned;

public:
    SpawnerComponent(const std::string& prefabGuid, int spawnCount, const std::vector<glm::vec3>& spawnPositions, bool spawnOnNavmesh, bool spawnOnAllCells)
        : _prefabGuid(prefabGuid), _spawnCount(spawnCount), _spawnPositions(spawnPositions), _spawnOnNavmesh(spawnOnNavmesh), _spawnOnAllCells(spawnOnAllCells), _hasSpawned(false) {}

    [[nodiscard]] std::string GetPrefabGuid() const noexcept { return _prefabGuid; }
    [[nodiscard]] int GetSpawnCount() const noexcept { return _spawnCount; }
    [[nodiscard]] std::vector<glm::vec3> GetSpawnPositions() const noexcept { return _spawnPositions; }
    [[nodiscard]] bool GetSpawnOnNavmesh() const noexcept { return _spawnOnNavmesh; }
    [[nodiscard]] bool GetSpawnOnAllCells() const noexcept { return _spawnOnAllCells; }
    [[nodiscard]] bool HasSpawned() const noexcept { return _hasSpawned; }

    void SetHasSpawned(bool spawned) noexcept { _hasSpawned = spawned; }
};
