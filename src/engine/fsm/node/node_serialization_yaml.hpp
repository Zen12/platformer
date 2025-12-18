
#pragma once
#include "node_serialization.hpp"
#include "yaml-cpp/node/node.h"
#include "../../renderer/ui/ui_page_action_serialization.hpp"
#include "../../renderer/ui/ui_page_action_serialization_yaml.hpp"
#include "../../renderer/ui/button_listener_action.hpp"
#include "../../renderer/ui/button_listener_action_serialization.hpp"
#include "../../renderer/ui/button_listener_action_serialization_yaml.hpp"
#include "../../renderer/ui/trigger_setter_button_listener_action.hpp"
#include "../../renderer/ui/trigger_setter_button_listener_action_serialization.hpp"
#include "../../renderer/ui/trigger_setter_button_listener_action_serialization_yaml.hpp"
#include "../../scene/load_scene_action.hpp"
#include "../../scene/load_scene_action_serialization.hpp"
#include "../../scene/load_scene_action_serialization_yaml.hpp"
#include "action/set_system_trigger_action.hpp"
#include "action/set_system_trigger_action_serialization.hpp"
#include "action/set_system_trigger_action_serialization_yaml.hpp"
#include "action/log_action.hpp"
#include "action/log_action_serialization.hpp"
#include "action/log_action_serialization_yaml.hpp"
#include "action/action_serialization_data.hpp"

namespace YAML {
    template <>
    struct convert<StateNodeSerialization> {

        static ActionSerializationData DecodeAction(const YAML::Node& node) {
            const auto type = node["type"].as<std::string>();
            ActionSerializationData data;
            data.Type = type;

            if (type == "load_ui_page") {
                const auto serialization = node.as<UIPageActionSerialization>();
                data.Param = serialization.UiPageGuid;
            } else if (type == "load_scene") {
                const auto serialization = node.as<LoadSceneActionSerialization>();
                data.Param = serialization.SceneGuid;
            } else if (type == "action_button_listener") {
                const auto serialization = node.as<ButtonListenerActionSerialization>();
                data.Param = serialization.ButtonId;
            } else if (type == "action_trigger_setter_button_listener") {
                const auto serialization = node.as<TriggerSetterButtonListenerActionSerialization>();
                data.Param = serialization.ButtonId;
                data.Param2 = serialization.TriggerName;
            } else if (type == "set_system_trigger") {
                const auto serialization = node.as<SetSystemTriggerActionSerialization>();
                data.Param = serialization.TriggerType;
            } else if (type == "log") {
                const auto serialization = node.as<LogActionSerialization>();
                data.Param = serialization.Message;
            }

            return data;
        }

        static bool decode(const Node &node, StateNodeSerialization &rhs) {
            rhs.Guid = node["guid"].as<std::string>();
            for (const auto& comp : node["actions"]) {
                rhs.ActionData.push_back(DecodeAction(comp));
            }
            return true;
        }
    };
}
