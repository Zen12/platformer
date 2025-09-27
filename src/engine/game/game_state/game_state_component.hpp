#pragma once
#include <memory>

#include "../team/team_repository.hpp"
#include "../../scene/scene.hpp"
#include "../../components/entity.hpp"


struct GameStateViewModel {
    bool IsValid;
    int TeamACount;
    int TeamBCount;
};

// This is temp solution, once we have FSM, we will refactor this to a proper state machine
// ATM the purpose of this is to give a feeling for completed gameplay logic
class GameStateController final : public Component{
private:
    std::shared_ptr<TeamRepository> _teamRepository{};
    std::weak_ptr<Scene> _scene{};

    std::string _winSceneId{};
    std::string _loseSceneId{};

private:
    void Win() const {
        if (const auto &sceneManager = _scene.lock()) {
            sceneManager->RequestToLoadScene(_winSceneId);
        }
    }

    void Lose() const {
        if (const auto &scene = _scene.lock()) {
            scene->RequestToLoadScene(_loseSceneId);
        }
    }

public:
    explicit GameStateController(const std::weak_ptr<Entity> &entity)
        : Component(entity), _teamRepository(std::make_shared<TeamRepository>()) {
    }

    [[nodiscard]] std::weak_ptr<TeamRepository> GetTeamRepository() const noexcept {
        return _teamRepository;
    }

    void SetScene(const std::weak_ptr<Scene> &scene) noexcept {
        _scene = scene;
    }

    void SetWinSceneId(const std::string &id) noexcept {
        _winSceneId = id;
    }

    void SetLoseSceneId(const std::string &id) noexcept {
        _loseSceneId = id;
    }

    [[nodiscard]] GameStateViewModel GetViewModel() const noexcept {
        return {
            .IsValid = _teamRepository->IsValidData(),
            .TeamACount = _teamRepository->GetCountTeamMembers(Team::A),
            .TeamBCount = _teamRepository->GetCountTeamMembers(Team::B)
        };
    }

    void Update([[maybe_unused]] const float &deltaTime) override {
        if (!_teamRepository->IsValidData())
            return;


        if (const auto scene = _scene.lock()) {
            if (_teamRepository->GetCountTeamMembers(Team::A) <= 0) {
                Lose();
                return;
            }

            if (_teamRepository->GetCountTeamMembers(Team::B) <= 0) {
                Win();
                return;
            }
        }
    }
};
