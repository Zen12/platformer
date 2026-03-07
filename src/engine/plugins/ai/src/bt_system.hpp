#pragma once
#include "esc/esc_core.hpp"
#include "bt_component.hpp"
#include "bt_executor.hpp"
#include "bt_asset.hpp"
#include "bt_asset_yaml.hpp"
#include "plugins/core/src/navmesh_agent/navmesh_agent_component.hpp"
#include "plugins/core/src/transform/transform_component.hpp"
#include "plugins/core/src/time/time_component.hpp"
#include "plugins/core/src/camera/camera_component.hpp"
#include "asset_manager.hpp"
#include "scene.hpp"
#include "navigation_manager.hpp"
#include "guid.hpp"
#include "combat_state/combat_state_component.hpp"
#include <unordered_map>

#define BT_SYSTEM_DEBUG 0

#if BT_SYSTEM_DEBUG
#include <iostream>
#define BT_SYSTEM_LOG(msg) std::cerr << "[BT_SYSTEM] " << msg << std::endl
#else
#define BT_SYSTEM_LOG(msg)
#endif

class BehaviorTreeSystem final : public ISystemView<BehaviorTreeComponent> {
private:
    entt::registry& _registry;
    std::weak_ptr<Scene> _scene;
    decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) _timeView;
    decltype(std::declval<entt::registry&>().view<CameraComponentV2, TransformComponentV2>()) _cameraView;
    uint32_t _frameCount = 0;

    std::unordered_map<Guid, std::shared_ptr<BehaviorTreeDef>> _treeCache;

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

        glm::vec3 cameraPos(0);
        for (auto [entity, cam, camTransform] : _cameraView.each()) {
            cameraPos = camTransform.GetPosition();
            break;
        }

        GridNavmesh* navmesh = nullptr;
        if (auto scene = _scene.lock()) {
            if (auto navManager = scene->GetNavigationManager()) {
                if (auto navmeshPtr = navManager->GetNavmesh()) {
                    navmesh = navmeshPtr.get();
                }
            }
        }

        for (auto [entity, bt] : View.each()) {
            if (!bt.TreeDef && !bt.TreeGuid.IsEmpty()) {
                bt.TreeDef = LoadTree(bt.TreeGuid);
                if (bt.TreeDef) {
                    bt.State.Reset();
                    bt.FrameOffset = static_cast<uint8_t>(static_cast<uint32_t>(entity) % 8);
                }
            }

            if (!bt.TreeDef) continue;

            uint32_t tickRate = 1u << bt.TickPriority;
            if ((_frameCount + bt.FrameOffset) % tickRate != 0) {
                continue;
            }

            if (_registry.all_of<TransformComponentV2>(entity)) {
                auto& transform = _registry.get<TransformComponentV2>(entity);
                float dist = glm::distance(transform.GetPosition(), cameraPos);

                if (dist > 100.0f && bt.TickPriority < 3) {
                    bt.TickPriority = 3;
                } else if (dist > 50.0f && bt.TickPriority < 2) {
                    bt.TickPriority = 2;
                } else if (dist > 20.0f && bt.TickPriority < 1) {
                    bt.TickPriority = 1;
                } else if (dist <= 20.0f) {
                    bt.TickPriority = 0;
                }
            }

            BTExecutor::Execute(bt, deltaTime, _registry, entity, navmesh);

            // Sync combat state to CombatStateComponent
            if (_registry.all_of<CombatStateComponent>(entity)) {
                auto& combatState = _registry.get<CombatStateComponent>(entity);
                combatState.IsAttacking = bt.IsAttacking;
                combatState.AttackTimer = bt.AttackTimer;
            } else if (bt.IsAttacking) {
                _registry.emplace<CombatStateComponent>(entity, CombatStateComponent{bt.IsAttacking, bt.AttackTimer});
            }
        }
    }

private:
    std::shared_ptr<BehaviorTreeDef> LoadTree(const Guid& guid) {
        auto it = _treeCache.find(guid);
        if (it != _treeCache.end()) {
            return it->second;
        }

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
            BT_SYSTEM_LOG("Failed to load behavior tree: " << guid.ToString() << " - " << e.what());
            return nullptr;
        }
    }
};
