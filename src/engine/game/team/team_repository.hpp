#pragma once

#include "../../entity/entity.hpp"
#include "team_data.hpp"

class TeamRepository {
private:
    std::unordered_map<Team, int> _teams{};

public:

    void OnIncreaseTeamMember(const Team &team) {
        if (const auto it = _teams.find(team); it != _teams.end()) {
            it->second++;
        } else {
            _teams.emplace(team, 1);
        }
    }

    void OnDecreaseTeamMember(const Team &team) {
        if (const auto it = _teams.find(team); it != _teams.end()) {
            it->second--;
        } else {
            _teams.emplace(team, 0);
        }
    }

    [[nodiscard]] bool IsValidData() {
        if (_teams.find(Team::A) == _teams.end() || _teams.find(Team::B) == _teams.end()) {
            return false;
        }
        return true;
    }

    [[nodiscard]] int GetCountTeamMembers(const Team &team) const {
        if (const auto it = _teams.find(team); it != _teams.end()) {
            return it->second;
        }

        return -1;
    }

};