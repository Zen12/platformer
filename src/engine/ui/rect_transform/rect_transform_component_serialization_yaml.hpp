#pragma once
#include <yaml-cpp/yaml.h>
#include "rect_transform_component_serialization.hpp"

namespace YAML {
    template <>
    struct convert<RectTransformComponentSerialization>
    {
        static void decodeLayout(const std::string& key, const Node &node, RectTransformComponentSerialization &rhs) {
            auto x = 0.0f;
            auto y = 0.0f;
            auto value =  0.0f;
            for (const auto &item : node[key]) {
                x = item["x"] ? item["x"].as<float>() : x;
                y = item["y"] ? item["y"].as<float>() : y;
                value = item["value"] ? item["value"].as<float>() : value;
            }
            rhs.Layouts.emplace_back(key, value, x, y);
        }
        static bool decode(const Node &node, RectTransformComponentSerialization &rhs) {
            if (const auto nodeValue = node["pixel_x"]) {
                decodeLayout("pixel_x", node, rhs);
            }
            if (const auto nodeValue = node["pixel_y"]) {
                decodeLayout("pixel_y", node, rhs);
            }
            if (const auto nodeValue = node["center_x"]) {
                decodeLayout("center_x", node, rhs);
            }
            if (const auto nodeValue = node["center_y"]) {
                decodeLayout("center_y", node, rhs);
            }
            if (const auto nodeValue = node["pixel_width"]) {
                decodeLayout("pixel_width", node, rhs);
            }
            if (const auto nodeValue = node["pixel_height"]) {
                decodeLayout("pixel_height", node, rhs);
            }
            if (const auto nodeValue = node["pivot"]) {
                decodeLayout("pivot", node, rhs);
            }
            return true;
        }
    };
}