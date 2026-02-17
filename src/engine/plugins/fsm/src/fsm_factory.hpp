#pragma once

#include <memory>
#include "fsm_controller.hpp"
#include "fsm_builder.hpp"
#include "fsm_asset.hpp"
#include "trigger_board.hpp"
#include "action_registry.hpp"
#include "condition_registry.hpp"
#include "engine_context.hpp"

class FsmFactory final {
public:
    static std::shared_ptr<FsmController> Create(
        const FsmAsset& fsmAsset,
        const ActionRegistry& actionRegistry,
        const ConditionRegistry& conditionRegistry,
        EngineContext context
    ) {
        auto triggerBoard = std::make_shared<TriggerBoard>();
        context.Register<TriggerBoard>("TriggerBoard", triggerBoard);

        FsmData data = FsmBuilder::Build(
            fsmAsset,
            actionRegistry,
            conditionRegistry,
            context
        );

        return std::make_shared<FsmController>(std::move(data), triggerBoard);
    }
};
