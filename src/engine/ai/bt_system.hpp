#pragma once
#include "../esc/esc_core.hpp"
#include "bt_component.hpp"
#include "bt_executor.hpp"
#include "bt_asset.hpp"
#include "bt_asset_yaml.hpp"
#include "../esc/navmesh_agent/navmesh_agent_component.hpp"
#include "../esc/transform/transform_component.hpp"
#include "../esc/time/time_component.hpp"
#include "../esc/camera/camera_component.hpp"
#include "../asset/asset_manager.hpp"
#include "../scene/scene.hpp"
#include "../navigation/navigation_manager.hpp"
#include <unordered_map>
#include <iostream>

class BehaviorTreeSystem final : public ISystemView<BehaviorTreeComponent> {
private:
    entt::registry& _registry;
    std::weak_ptr<Scene> _scene;
    decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) _timeView;
    decltype(std::declval<entt::registry&>().view<CameraComponentV2, TransformComponentV2>()) _cameraView;
    uint32_t _frameCount = 0;

    // Cache of loaded behavior trees (shared between entities)
    std::unordered_map<std::string, std::shared_ptr<BehaviorTreeDef>> _treeCache;

public:
    BehaviorTreeSystem(
        TypeView view,
        entt::registry& registry,
        std::weak_ptr<Scene> scene,
        decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) timeView,
        decltype(std::declval<entt::registry&>().view<CameraComponentV2, TransformComponentV2>()) cameraView
    )
        : ISystemView(view), _registry(registry), _scene(std::move(scene)), _timeView(timeView), _cameraView(cameraView) {}

    void OnTick() override {
        _frameCount++;

        float deltaTime = 0.0f;
        for (auto [entity, time] : _timeView.each()) {
            deltaTime = time.Delta;
        }

        // Get camera position for LOD
        glm::vec3 cameraPos(0);
        for (auto [entity, cam, camTransform] : _cameraView.each()) {
            cameraPos = camTransform.GetPosition();
            break;
        }

        // Get navmesh for path validation
        GridNavmesh* navmesh = nullptr;
        if (auto scene = _scene.lock()) {
            if (auto navManager = scene->GetNavigationManager()) {
                if (auto navmeshPtr = navManager->GetNavmesh()) {
                    navmesh = navmeshPtr.get();
                }
            }
        }

        for (auto [entity, bt] : View.each()) {
            // Lazy load tree definition if needed
            if (!bt.TreeDef && !bt.TreeGuid.empty()) {
                bt.TreeDef = LoadTree(bt.TreeGuid);
                if (bt.TreeDef) {
                    bt.State.Reset();
                    // Assign frame offset for staggered execution
                    bt.FrameOffset = static_cast<uint8_t>(static_cast<uint32_t>(entity) % 8);
                }
            }

            if (!bt.TreeDef) continue;

            // LOD: Skip based on tick priority and frame offset
            uint32_t tickRate = 1u << bt.TickPriority;  // 1, 2, 4, 8...
            if ((_frameCount + bt.FrameOffset) % tickRate != 0) {
                continue;
            }

            // Dynamic LOD based on distance to camera
            if (_registry.all_of<TransformComponentV2>(entity)) {
                auto& transform = _registry.get<TransformComponentV2>(entity);
                float dist = glm::distance(transform.GetPosition(), cameraPos);

                // Adjust tick priority based on distance
                if (dist > 100.0f && bt.TickPriority < 3) {
                    bt.TickPriority = 3;  // Far: every 8 frames
                } else if (dist > 50.0f && bt.TickPriority < 2) {
                    bt.TickPriority = 2;  // Medium: every 4 frames
                } else if (dist > 20.0f && bt.TickPriority < 1) {
                    bt.TickPriority = 1;  // Near: every 2 frames
                } else if (dist <= 20.0f) {
                    bt.TickPriority = 0;  // Close: every frame
                }
            }

            // Get optional components for execution
            NavmeshAgentComponent* navAgent = nullptr;
            TransformComponentV2* transform = nullptr;

            if (_registry.all_of<NavmeshAgentComponent>(entity)) {
                navAgent = &_registry.get<NavmeshAgentComponent>(entity);
            }
            if (_registry.all_of<TransformComponentV2>(entity)) {
                transform = &_registry.get<TransformComponentV2>(entity);
            }

            BTExecutor::Execute(bt, navAgent, transform, deltaTime, _registry, entity, navmesh);
        }
    }

private:
    std::shared_ptr<BehaviorTreeDef> LoadTree(const std::string& guid) {
        // Check cache first
        auto it = _treeCache.find(guid);
        if (it != _treeCache.end()) {
            return it->second;
        }

        // Load from asset manager
        auto scene = _scene.lock();
        if (!scene) return nullptr;

        auto assetManager = scene->GetAssetManager().lock();
        if (!assetManager) return nullptr;

        try {
            auto btAsset = assetManager->LoadAssetByGuid<BehaviorTreeAsset>(guid);
            auto treeDef = std::make_shared<BehaviorTreeDef>(std::move(btAsset.Tree));
            _treeCache[guid] = treeDef;
            return treeDef;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load behavior tree: " << guid << " - " << e.what() << std::endl;
            return nullptr;
        }
    }
};
