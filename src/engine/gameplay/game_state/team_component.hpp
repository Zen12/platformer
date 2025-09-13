#pragma once

#include "../../components/entity.hpp"
#include "team_repository.hpp"

class TeamComponent final : public Component {
private:
    Team _team{};
    std::weak_ptr<TeamRepository> _repository;

public:
    explicit TeamComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SetTeamRepository(const std::weak_ptr<TeamRepository> &repository) noexcept{
        _repository = repository;
    }

    void SetTeam(const Team &team) noexcept {
        _team = team;
        if (const auto repo = _repository.lock()) {
            repo->OnIncreaseTeamMember(_team);
        }
    }

    void Update([[maybe_unused]] const float &deltaTime) override {

    }

    ~TeamComponent() override {
        if (const auto repo = _repository.lock()) {
            repo->OnDecreaseTeamMember(_team);
        }
    }
};
