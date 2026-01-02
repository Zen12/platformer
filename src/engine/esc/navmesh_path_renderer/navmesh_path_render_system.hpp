#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "../esc_core.hpp"
#include "../camera/camera_component.hpp"
#include "../navmesh_agent/navmesh_agent_component.hpp"
#include "../../renderer/render_repository.hpp"
#include "../../navigation/navigation_manager.hpp"

class NavmeshPathRenderSystem final : public ISystemView<NavmeshAgentComponent> {
private:
    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());

    const TypeCamera _cameraView;
    std::shared_ptr<RenderRepository> _repository;
    std::shared_ptr<NavigationManager> _navigationManager;
    std::string _materialGuid = "b53dc63c-0c57-46b1-80e5-7570f2445f8a"; // line.mat

    [[nodiscard]] std::vector<float> CalculatePathVertices() const noexcept {
        if (!_navigationManager) {
            return {};
        }

        auto crowd = _navigationManager->GetCrowd();
        if (!crowd) {
            return {};
        }

        std::vector<float> vertices;

        for (const auto &[_, agent] : View.each()) {
            if (!agent.Enabled || agent.CrowdAgentId == -1) {
                continue;
            }

            auto crowdAgent = crowd->GetAgent(agent.CrowdAgentId);
            if (!crowdAgent || crowdAgent->Path.empty()) {
                continue;
            }

            // Draw lines connecting path waypoints
            for (size_t i = 0; i < crowdAgent->Path.size() - 1; ++i) {
                const auto& p1 = crowdAgent->Path[i];
                const auto& p2 = crowdAgent->Path[i + 1];

                vertices.push_back(p1.x);
                vertices.push_back(p1.y + 0.5f);
                vertices.push_back(p1.z);

                vertices.push_back(p2.x);
                vertices.push_back(p2.y + 0.5f);
                vertices.push_back(p2.z);
            }

            // Draw line from agent position to first waypoint
            if (!crowdAgent->Path.empty()) {
                vertices.push_back(crowdAgent->Position.x);
                vertices.push_back(crowdAgent->Position.y + 0.5f);
                vertices.push_back(crowdAgent->Position.z);

                vertices.push_back(crowdAgent->Path[0].x);
                vertices.push_back(crowdAgent->Path[0].y + 0.5f);
                vertices.push_back(crowdAgent->Path[0].z);
            }
        }

        return vertices;
    }

public:
    NavmeshPathRenderSystem(
        const TypeView &view,
        const TypeCamera &cameraView,
        const std::shared_ptr<RenderRepository> &repository,
        const std::shared_ptr<NavigationManager> &navigationManager)
        : ISystemView(view), _cameraView(cameraView),
          _repository(repository), _navigationManager(navigationManager) {}

    void OnTick() override {
        const auto vertices = CalculatePathVertices();

        if (vertices.empty()) {
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
                vertices
            });
        }
    }
};
