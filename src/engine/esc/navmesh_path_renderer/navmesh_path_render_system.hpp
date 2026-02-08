#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "../esc_core.hpp"
#include "../camera/camera_component.hpp"
#include "../navmesh_agent/navmesh_agent_component.hpp"
#include "../../renderer/render_repository.hpp"
#include "../../navigation/navigation_manager.hpp"
#include "../../system/guid.hpp"

class NavmeshPathRenderSystem final : public ISystemView<NavmeshAgentComponent> {
private:
    using TypeCamera = decltype(std::declval<entt::registry>().view<CameraComponentV2>());

    const TypeCamera _cameraView;
    std::shared_ptr<RenderRepository> _repository;
    std::shared_ptr<NavigationManager> _navigationManager;
    Guid _materialGuid = Guid::FromString("b53dc63c-0c57-46b1-80e5-7570f2445f8a");

    [[nodiscard]] std::vector<glm::vec3> CalculatePathPositions() const noexcept {
        if (!_navigationManager) {
            return {};
        }

        auto crowd = _navigationManager->GetCrowd();
        auto navmesh = _navigationManager->GetNavmesh();
        if (!crowd || !navmesh) {
            return {};
        }

        std::vector<glm::vec3> positions;

        for (const auto &[_, agent] : View.each()) {
            if (!agent.Enabled || !agent.HasDestination || agent.CrowdAgentId < 0) {
                continue;
            }

            const auto& path = agent.PathWaypoints;

            if (path.size() >= 2) {
                const int startIdx = std::max(0, agent.CurrentWaypointIndex);
                for (size_t i = startIdx; i < path.size() - 1; ++i) {
                    positions.emplace_back(path[i].x, path[i].y + 0.5f, path[i].z);
                    positions.emplace_back(path[i + 1].x, path[i + 1].y + 0.5f, path[i + 1].z);
                }
            } else if (!path.empty()) {
                const glm::vec3 agentPos = crowd->GetAgentPosition(agent.CrowdAgentId);
                positions.emplace_back(agentPos.x, agentPos.y + 0.5f, agentPos.z);
                positions.emplace_back(agent.Destination.x, agent.Destination.y + 0.5f, agent.Destination.z);
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
                Guid{},
                glm::mat4(1.0f),
                camera.View,
                camera.Projection,
                std::nullopt,
                PrimitiveType::Lines,
                positions,
                glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                std::nullopt,
                false
            });
        }
    }
};
