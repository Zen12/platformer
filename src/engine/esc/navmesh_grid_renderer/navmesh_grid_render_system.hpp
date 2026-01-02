#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "../esc_core.hpp"
#include "../camera/camera_component.hpp"
#include "../../renderer/render_repository.hpp"
#include "../../navigation/navigation_manager.hpp"
#include "../../scene/scene.hpp"

class NavmeshGridRenderSystem final : public ISystem {
private:
    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());

    const TypeCamera _cameraView;
    std::weak_ptr<Scene> _scene;
    std::shared_ptr<RenderRepository> _repository;
    std::string _materialGuid = "b53dc63c-0c57-46b1-80e5-7570f2445f8a"; // line.mat
    std::vector<float> _gridVertices;

    void CalculateGridVertices() noexcept {
        if (const auto scene = _scene.lock()) {
            if (const auto navManager = scene->GetNavigationManager()) {
                if (const auto navmesh = navManager->GetNavmesh()) {
                    const int width = navmesh->GetWidth();
                    const int height = navmesh->GetHeight();
                    const float cellSize = navmesh->GetCellSize();
                    const glm::vec3 origin = navmesh->GetOrigin();

                    _gridVertices.clear();

                    for (int x = 0; x <= width; ++x) {
                        for (int z = 0; z <= height; ++z) {
                            if (navmesh->IsWalkable(x, z)) {
                                const float worldX = origin.x + static_cast<float>(x) * cellSize;
                                const float worldZ = origin.z + static_cast<float>(z) * cellSize;

                                _gridVertices.push_back(worldX);
                                _gridVertices.push_back(0.0f);
                                _gridVertices.push_back(worldZ);

                                _gridVertices.push_back(worldX);
                                _gridVertices.push_back(3.0f);
                                _gridVertices.push_back(worldZ);
                            }
                        }
                    }
                }
            }
        }
    }

public:
    NavmeshGridRenderSystem(
        const TypeCamera &cameraView,
        const std::weak_ptr<Scene> &scene,
        const std::shared_ptr<RenderRepository> &repository)
        : _cameraView(cameraView), _scene(scene), _repository(repository) {
        CalculateGridVertices();
    }

    void OnTick() override {
        if (_gridVertices.empty()) {
            return;
        }

        for (const auto &[_, camera] : _cameraView.each()) {
            _repository->Add(RenderData{
                _materialGuid,
                "",  // No mesh GUID needed for direct vertex rendering
                glm::mat4(1.0f),
                camera.View,
                camera.Projection,
                std::nullopt,
                PrimitiveType::Lines,
                _gridVertices
            });
        }
    }
};
