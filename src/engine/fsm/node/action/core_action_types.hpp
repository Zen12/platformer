#pragma once
#include "set_system_trigger_action.hpp"
#include "log_action.hpp"
#include "health_check_action.hpp"
#include "../../type_list.hpp"

using CoreActionTypes = fsm::TypeList<
    SetSystemTriggerAction,
    LogAction,
    HealthCheckAction
>;
