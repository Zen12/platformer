#pragma once
#include "animation_state_action.hpp"
#include "animation_state_transition_action.hpp"
#include "../../type_list.hpp"

using AnimationActionTypes = fsm::TypeList<
    AnimationStateAction,
    AnimationStateTransitionAction
>;
