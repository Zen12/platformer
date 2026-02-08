#pragma once
#include <memory>
#include <iostream>
#include <variant>

#define DEBUG_FSM_CONTROLLER 0

#if DEBUG_FSM_CONTROLLER
#define FSM_LOG if(1) std::cout
#else
#define FSM_LOG if(0) std::cout
#endif

#include "fsm_data.hpp"
#include "trigger_board.hpp"

class FsmController final {
private:
    const FsmData _data;
    std::shared_ptr<TriggerBoard> _triggerBoard;

    std::string _currentState;
    bool _wasEntered;

private:
    void ChangeState(const std::string &state) {
        FSM_LOG << "[FSM] State transition: " << _currentState << " -> " << state << std::endl;

        const auto& currentNode = _data.StateNodes.at(_currentState);
        currentNode.ExitAll();

        _currentState = state;
        _wasEntered = false;
        _triggerBoard->ClearTriggers();
    }

public:
    FsmController(FsmData data, std::shared_ptr<TriggerBoard> triggerBoard)
        : _data(std::move(data)),
          _triggerBoard(std::move(triggerBoard)),
          _currentState(_data.StartNode),
          _wasEntered(false)
    {}

    [[nodiscard]] std::shared_ptr<TriggerBoard> GetTriggerBoard() const {
        return _triggerBoard;
    }

    [[nodiscard]] bool IsSystemTriggered(SystemTriggers trigger) const {
        return _triggerBoard->IsSystemTriggered(trigger);
    }

    void SetSystemTrigger(SystemTriggers trigger) {
        _triggerBoard->SetSystemTrigger(trigger);
    }

    void SetTrigger(const std::string& triggerName) {
        FSM_LOG << "[FSM] Trigger set: " << triggerName << " (current state: " << _currentState << ")" << std::endl;
        _triggerBoard->SetTrigger(triggerName);
    }

    void Update() {
        const auto& currentNode = _data.StateNodes.at(_currentState);

        if (!_wasEntered) {
            FSM_LOG << "[FSM] Entering state: " << _currentState << std::endl;
            currentNode.EnterAll();
            _wasEntered = true;
        }

        currentNode.UpdateAll();

        for (const auto& conn : _data.Connections) {
            if (conn.Nodes[0] != _currentState) {
                continue;
            }

            bool conditionsPassed = false;

            auto checkCondition = [](const AllConditionVariants& cond) {
                return std::visit([](const auto& c) { return c.IsSuccess(); }, cond);
            };

            if (conn.ConditionType == ConditionType::All) {
                conditionsPassed = true;
                for (const auto &conditionGuid: conn.ConditionGuids) {
                    auto condIt = _data.Conditions.find(conditionGuid);
                    if (condIt == _data.Conditions.end() || !checkCondition(condIt->second)) {
                        conditionsPassed = false;
                        break;
                    }
                }
            } else if (conn.ConditionType == ConditionType::AtLeastOne) {
                for (const auto &conditionGuid: conn.ConditionGuids) {
                    auto condIt = _data.Conditions.find(conditionGuid);
                    if (condIt != _data.Conditions.end() && checkCondition(condIt->second)) {
                        conditionsPassed = true;
                        break;
                    }
                }
            }

            if (conditionsPassed) {
                ChangeState(conn.Nodes[1]);
                return;
            }
        }
    }
};
