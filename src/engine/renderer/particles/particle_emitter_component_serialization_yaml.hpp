#pragma once
#include <yaml-cpp/yaml.h>
#include "particle_emitter_component_serialization.hpp"

namespace YAML
{
    template <>
    struct convert<ParticleEmitterSerialization>
    {
        static bool decode(const Node &node, ParticleEmitterSerialization &rhs) {
            rhs.count = node["count"].as<size_t>();
            rhs.startVelocity = node["start_velocity"].as<glm::vec3>();
            rhs.endVelocity = node["end_velocity"].as<glm::vec3>();
            rhs.duration = node["duration"].as<float>();
            rhs.materialGuid = node["material_guid"].as<std::string>();
            rhs.startScale = node["start_scale"].as<float>();
            rhs.endScale = node["end_scale"].as<float>();
            rhs.startColor = node["start_color"].as<glm::vec4>();
            rhs.endColor = node["end_color"].as<glm::vec4>();
            return true;
        }
    };
}