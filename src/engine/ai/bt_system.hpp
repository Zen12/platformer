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
#include "../system/guid.hpp"
#include <unordered_map>
#include <iostream>

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
            std::cerr << "Failed to load behavior tree: " << guid.ToString() << " - " << e.what() << std::endl;
            return nullptr;
        }
    }
};
