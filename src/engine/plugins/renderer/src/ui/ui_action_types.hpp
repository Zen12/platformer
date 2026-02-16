#pragma once
#include "ui_page_action.hpp"
#include "button_listener_action.hpp"
#include "trigger_setter_button_listener_action.hpp"
#include "type_list.hpp"

using UiActionTypes = fsm::TypeList<
    UiPageAction,
    ButtonListenerAction,
    TriggerSetterButtonListenerAction
>;
