
#pragma once
#include "node_serialization.hpp"
#include "yaml-cpp/node/node.h"
#include "../../renderer/ui/ui_page_action_serialization.hpp"
#include "../../renderer/ui/ui_page_action_serialization_yaml.hpp"
#include "../../scene/load_scene_action.hpp"
#include "../../scene/load_scene_action_serialization.hpp"
#include "../../scene/load_scene_action_serialization_yaml.hpp"

namespace YAML {
    template <>
    struct convert<StateNodeSerialization> {

        static StateNode::AllActionVariants DecodeAction(const YAML::Node& node) {
            const auto type = node["type"].as<std::string>();

            if (type == "load_ui_page") {
                const auto serialization = node.as<UIPageActionSerialization>();
                UiPageAction action;
                action.SetPageGuid(serialization.UiPageGuid);
                // Note: UIManager will be set later by the FsmController
                return action;
            } else if (type == "load_scene") {
                const auto serialization = node.as<LoadSceneActionSerialization>();
                LoadSceneAction action;
                action.SetSceneGuid(serialization.SceneGuid);
                // Note: SceneManager will be set later by the FsmController
                return action;
            }

            // Return default-constructed action if type not found
            return UiPageAction();
        }
        static bool decode(const Node &node, StateNodeSerialization &rhs) {

            rhs.Guid = node["guid"].as<std::string>();
            for (const auto& comp : node["actions"]) {
                rhs.Actions.push_back(DecodeAction(comp));
            }


            return true;
        }
    };
}
