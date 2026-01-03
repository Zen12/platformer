#pragma once
#include "../esc_core.hpp"
#include "spawner_component.hpp"
#include "../../scene/scene.hpp"
#include "../../asset/asset_manager.hpp"
#include "../../prefab/prefab_asset.hpp"
#include "../../prefab/prefab_asset_yaml.hpp"
#include "../transform/transform_component.hpp"
#include "../transform/transform_component_serialization.hpp"
#include "../mesh_renderer/mesh_renderer_component_serialization.hpp"
#include "../skinned_mesh_renderer/skinned_mesh_renderer_component.hpp"
#include "../skinned_mesh_renderer/skinned_mesh_renderer_component_serialization.hpp"
#include "../animation/simple_animation_component.hpp"
#include "../animation/simple_animation_component_serialization.hpp"
#include "../animation/fsm_animation_component.hpp"
#include "../animation/fsm_animation_component_serialization.hpp"
#include "../navmesh_agent/navmesh_agent_component.hpp"
#include "../navmesh_agent/navmesh_agent_component_serialization.hpp"
#include "../navmesh_agent/random_navigation_component.hpp"
#include "../camera/camera_component.hpp"
#include "../camera/camera_component_serialization.hpp"
#include "../camera/camera_controller_component.hpp"
#include "../camera/camera_controller_component_serialization.hpp"
#include "../tag/tag_component.hpp"
#include <memory>
#include <random>

class SpawnerSystem final : public ISystemView<SpawnerComponent> {
private:
    std::weak_ptr<Scene> _scene;
    std::mt19937 _rng;
    std::uniform_real_distribution<float> _dist;

    void SpawnEntity(const std::shared_ptr<entt::registry>& registry, const EntitySerialization& prefabData, const glm::vec3& position) {
        const auto entity = registry->create();
        const auto tagView = registry->view<TagComponent>();
        tagView->emplace(entity, prefabData.Tag);

        for (const auto& component : prefabData.Components) {
            if (const auto transformSerialization = dynamic_cast<TransformComponentSerialization*>(component.get())) {
                const auto view = registry->view<TransformComponentV2>();
                TransformComponentV2 transform(*transformSerialization);
                transform.SetPosition(position);
                view->emplace(entity, transform);
            } else if (const auto meshSerialization = dynamic_cast<MeshRendererComponentSerialization*>(component.get())) {
                const auto view = registry->view<MeshRendererComponent>();
                view->emplace(entity, *meshSerialization);
            } else if (const auto skinnedMeshSerialization = dynamic_cast<SkinnedMeshRendererComponentSerialization*>(component.get())) {
                const auto view = registry->view<SkinnedMeshRendererComponent>();
                view->emplace(entity, *skinnedMeshSerialization);
            } else if (const auto animationSerialization = dynamic_cast<SimpleAnimationComponentSerialization*>(component.get())) {
                const auto view = registry->view<SimpleAnimationComponent>();
                SimpleAnimationComponent animComp(animationSerialization->AnimationGuid);
                view->emplace(entity, animComp);
            } else if (const auto fsmAnimationSerialization = dynamic_cast<FsmAnimationComponentSerialization*>(component.get())) {
                const auto view = registry->view<FsmAnimationComponent>();
                FsmAnimationComponent fsmAnimComp(fsmAnimationSerialization->FsmGuid);
                fsmAnimComp.Loop = fsmAnimationSerialization->Loop;
                view->emplace(entity, fsmAnimComp);
            } else if (const auto navmeshAgentSerialization = dynamic_cast<NavmeshAgentComponentSerialization*>(component.get())) {
                const auto view = registry->view<NavmeshAgentComponent>();
                view->emplace(entity, *navmeshAgentSerialization);

                const auto randomNavView = registry->view<RandomNavigationComponent>();
                randomNavView->emplace(entity, RandomNavigationComponent());
            } else if (const auto cameraSerialization = dynamic_cast<CameraComponentSerialization*>(component.get())) {
                const auto view = registry->view<CameraComponentV2>();
                view->emplace(entity, *cameraSerialization);
            } else if (const auto cameraControllerSerialization = dynamic_cast<CameraControllerComponentSerialization*>(component.get())) {
                const auto view = registry->view<CameraControllerComponent>();
                CameraControllerComponent controller;
                controller.SetMoveSpeed(cameraControllerSerialization->MoveSpeed);
                controller.SetMouseSensitivity(cameraControllerSerialization->MouseSensitivity);
                view->emplace(entity, controller);
            }
        }
    }

public:
    SpawnerSystem(TypeView view, const std::weak_ptr<Scene>& scene)
        : ISystemView(view), _scene(scene), _rng(std::random_device{}()), _dist(0.0f, 1.0f) {}

    void OnTick() override {
        if (const auto scenePtr = _scene.lock()) {
            const auto registry = scenePtr->GetEntityRegistry();
            const auto assetManagerWeak = scenePtr->GetAssetManager();

            if (!registry) return;

            const auto assetManager = assetManagerWeak.lock();
            if (!assetManager) return;

            for (auto [entity, spawner] : View.each()) {
                if (spawner.HasSpawned()) continue;

                const auto prefabAsset = assetManager->LoadAssetByGuid<PrefabAsset>(spawner.GetPrefabGuid());
                const auto& prefabData = prefabAsset.Obj;

                const auto spawnPositions = spawner.GetSpawnPositions();
                const int spawnCount = spawner.GetSpawnCount();
                const bool spawnOnNavmesh = spawner.GetSpawnOnNavmesh();
                const bool spawnOnAllCells = spawner.GetSpawnOnAllCells();

                if (!spawnPositions.empty()) {
                    for (int i = 0; i < spawnCount && i < static_cast<int>(spawnPositions.size()); ++i) {
                        SpawnEntity(registry, prefabData, spawnPositions[i]);
                    }
                } else if (spawnOnAllCells) {
                    const auto navManager = scenePtr->GetNavigationManager();
                    if (navManager) {
                        const auto navmesh = navManager->GetNavmesh();
                        if (navmesh) {
                            const glm::vec3 origin = navmesh->GetOrigin();
                            const float cellSize = navmesh->GetCellSize();
                            const int width = navmesh->GetWidth();
                            const int height = navmesh->GetHeight();

                            for (int z = 0; z < height; ++z) {
                                for (int x = 0; x < width; ++x) {
                                    if (navmesh->IsWalkable(x, z)) {
                                        const float worldX = origin.x + (x * cellSize) + (cellSize * 0.5f);
                                        const float worldZ = origin.z + (z * cellSize) + (cellSize * 0.5f);
                                        const glm::vec3 cellCenter(worldX, 0.0f, worldZ);
                                        SpawnEntity(registry, prefabData, cellCenter);
                                    }
                                }
                            }
                        }
                    }
                } else if (spawnOnNavmesh) {
                    const auto navManager = scenePtr->GetNavigationManager();
                    if (navManager) {
                        const auto navmesh = navManager->GetNavmesh();
                        if (navmesh) {
                            const glm::vec3 origin = navmesh->GetOrigin();
                            const float cellSize = navmesh->GetCellSize();
                            const int width = navmesh->GetWidth();
                            const int height = navmesh->GetHeight();

                            const float minX = origin.x;
                            const float maxX = origin.x + width * cellSize;
                            const float minZ = origin.z;
                            const float maxZ = origin.z + height * cellSize;

                            for (int i = 0; i < spawnCount; ++i) {
                                const float randX = minX + (maxX - minX) * _dist(_rng);
                                const float randZ = minZ + (maxZ - minZ) * _dist(_rng);
                                const glm::vec3 randomPos(randX, 0.0f, randZ);

                                const glm::vec3 walkablePos = navmesh->FindClosestWalkablePoint(randomPos);
                                SpawnEntity(registry, prefabData, walkablePos);
                            }
                        }
                    }
                } else {
                    // Spawn at origin if no positions specified and not spawning on navmesh
                    for (int i = 0; i < spawnCount; ++i) {
                        SpawnEntity(registry, prefabData, glm::vec3(0.0f, 0.0f, 0.0f));
                    }
                }

                spawner.SetHasSpawned(true);
            }
        }
    }
};
