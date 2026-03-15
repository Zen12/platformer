#pragma once
#include "ik_aim_component_serialization.hpp"
#include "glm_vec_yaml.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<IKAimComponentSerialization> {
        static bool decode(const Node &node, IKAimComponentSerialization &rhs) {
            if (node["enabled"]) {
                rhs.Enabled = node["enabled"].as<bool>();
            }
            if (node["blend_weight"]) {
                rhs.BlendWeight = node["blend_weight"].as<float>();
            }
            if (node["blend_speed"]) {
                rhs.BlendSpeed = node["blend_speed"].as<float>();
            }
            if (node["torso_max_yaw"]) {
                rhs.TorsoMaxYaw = node["torso_max_yaw"].as<float>();
            }
            if (node["torso_max_pitch"]) {
                rhs.TorsoMaxPitch = node["torso_max_pitch"].as<float>();
            }
            if (node["spine_weight"]) {
                rhs.SpineWeight = node["spine_weight"].as<float>();
            }
            if (node["spine1_weight"]) {
                rhs.Spine1Weight = node["spine1_weight"].as<float>();
            }
            if (node["chest_weight"]) {
                rhs.ChestWeight = node["chest_weight"].as<float>();
            }
            if (node["right_hand_offset"]) {
                rhs.RightHandOffset = node["right_hand_offset"].as<glm::vec3>();
            }
            if (node["left_hand_offset"]) {
                rhs.LeftHandOffset = node["left_hand_offset"].as<glm::vec3>();
            }
            if (node["left_hand_grip_point"]) {
                rhs.LeftHandGripPoint = node["left_hand_grip_point"].as<glm::vec3>();
            }
            if (node["right_elbow_hint"]) {
                rhs.RightElbowHint = node["right_elbow_hint"].as<glm::vec3>();
            }
            if (node["left_elbow_hint"]) {
                rhs.LeftElbowHint = node["left_elbow_hint"].as<glm::vec3>();
            }
            if (node["rotate_character_to_aim"]) {
                rhs.RotateCharacterToAim = node["rotate_character_to_aim"].as<bool>();
            }
            if (node["character_rotation_speed"]) {
                rhs.CharacterRotationSpeed = node["character_rotation_speed"].as<float>();
            }
            if (node["recoil_duration"]) {
                rhs.RecoilDuration = node["recoil_duration"].as<float>();
            }
            if (node["recoil_offset_right"]) {
                rhs.RecoilOffsetRight = node["recoil_offset_right"].as<glm::vec3>();
            }
            if (node["recoil_offset_left"]) {
                rhs.RecoilOffsetLeft = node["recoil_offset_left"].as<glm::vec3>();
            }
            return true;
        }
    };
}
