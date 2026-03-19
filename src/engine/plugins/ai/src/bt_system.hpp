#pragma once
#include "esc/esc_core.hpp"
#include "bt_component.hpp"
#include "bt_executor.hpp"
#include "bt_node_registry.hpp"
#include "bt_def.hpp"
#include "navmesh_agent/navmesh_agent_component.hpp"
#include "transform/transform_component.hpp"
#include "time/time_component.hpp"
#include "camera/camera_component.hpp"
#include "resource_factory.hpp"
#include "navigation_manager.hpp"
#include "navigation_manager_component.hpp"
#include "guid.hpp"
#include "combat_state/combat_state_component.hpp"

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
    std::shared_ptr<ResourceFactory> _resourceFactory;
    BTNodeRegistry _nodeRegistry;
    BTExecutor _executor;
    decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) _timeView;
    decltype(std::declval<entt::registry&>().view<CameraComponentV2, TransformComponentV2>()) _cameraView;
    uint32_t _frameCount = 0;

public:
    BehaviorTreeSystem(
        TypeView view,
        entt::registry& registry,
        std::shared_ptr<ResourceFactory> resourceFactory,
        decltype(std::declval<entt::registry&>().view<DeltaTimeComponent>()) timeView,
        decltype(std::declval<entt::registry&>().view<CameraComponentV2, TransformComponentV2>()) cameraView
    )
        : ISystemView(view), _registry(registry), _resourceFactory(std::move(resourceFactory)), _timeView(timeView), _cameraView(cameraView) {}

    void OnTick() override {
        _frameCount++;

        float deltaTime = 0.0f;
        for (auto [entity, time] : _timeView.each()) {
            deltaTime = time.FixedDelta;
        }

        glm::vec3 cameraPos(0);
        for (auto [entity, cam, camTransform] : _cameraView.each()) {
            cameraPos = camTransform.GetPosition();
            break;
        }

        GridNavmesh* navmesh = nullptr;
        for (const auto& [_, comp] : _registry.view<NavigationManagerComponent>().each()) {
            if (comp.Manager) {
                if (auto navmeshPtr = comp.Manager->GetNavmesh()) {
                    navmesh = navmeshPtr.get();
                }
            }
            break;
        }

        _executor.Set<float>(deltaTime);
        _executor.Set<entt::registry*>(&_registry);
        _executor.Set<GridNavmesh*>(navmesh);

        for (auto [entity, bt] : View.each()) {
            if (!bt.TreeDef && !bt.TreeGuid.IsEmpty()) {
                bt.TreeDef = _resourceFactory->Get<BehaviorTreeDef>(bt.TreeGuid);
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

            _executor.Execute(bt, _nodeRegistry, entity);

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

};
