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

    [[nodiscard]] std::vector<glm::vec3> CalculatePathPositions() const noexcept {
        if (!_navigationManager) {
            return {};
        }

        auto crowd = _navigationManager->GetCrowd();
        if (!crowd) {
            return {};
        }

        std::vector<glm::vec3> positions;

        for (const auto &[_, agent] : View.each()) {
            if (!agent.Enabled || agent.CrowdAgentId == -1) {
                continue;
            }

            auto crowdAgent = crowd->GetAgent(agent.CrowdAgentId);
            if (!crowdAgent || crowdAgent->Path.empty()) {
                continue;
            }

            // Only render remaining path (from CurrentWaypoint onwards)
            if (crowdAgent->CurrentWaypoint >= crowdAgent->Path.size()) {
                continue; // Agent reached destination
            }

            // Draw line from agent's current position to current waypoint
            const auto& currentWaypoint = crowdAgent->Path[crowdAgent->CurrentWaypoint];
            positions.emplace_back(crowdAgent->Position.x, crowdAgent->Position.y + 0.5f, crowdAgent->Position.z);
            positions.emplace_back(currentWaypoint.x, currentWaypoint.y + 0.5f, currentWaypoint.z);

            // Draw lines connecting remaining waypoints
            for (size_t i = crowdAgent->CurrentWaypoint; i < crowdAgent->Path.size() - 1; ++i) {
                const auto& p1 = crowdAgent->Path[i];
                const auto& p2 = crowdAgent->Path[i + 1];

                positions.emplace_back(p1.x, p1.y + 0.5f, p1.z);
                positions.emplace_back(p2.x, p2.y + 0.5f, p2.z);
            }
        }

        return positions;
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
        const auto positions = CalculatePathPositions();

        if (positions.empty()) {
            return;
        }

        for (const auto &[_, camera] : _cameraView.each()) {
            _repository->Add(RenderData{
                _materialGuid,
                "",  // No mesh GUID needed for direct position rendering
                glm::mat4(1.0f),
                camera.View,
                camera.Projection,
                std::nullopt,
                PrimitiveType::Lines,
                positions,
                glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)  // Green color for AI agent paths
            });
        }
    }
};
