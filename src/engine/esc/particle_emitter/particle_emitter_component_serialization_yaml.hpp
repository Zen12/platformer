#pragma once
#include "particle_emitter_component_serialization.hpp"
#include <yaml-cpp/yaml.h>

namespace YAML {
    template <>
    struct convert<ParticleEmitterComponentSerialization> {
        static bool decode(const Node& node, ParticleEmitterComponentSerialization& rhs) {
            rhs.MaterialGuid = node["material_guid"] ? node["material_guid"].as<std::string>() : "";
            rhs.MeshGuid = node["mesh_guid"] ? node["mesh_guid"].as<std::string>() : "";
            rhs.MaxParticles = node["max_particles"] ? node["max_particles"].as<int>() : 100;
            rhs.EmissionRate = node["emission_rate"] ? node["emission_rate"].as<float>() : 10.0f;
            rhs.ParticleLifetime = node["particle_lifetime"] ? node["particle_lifetime"].as<float>() : 2.0f;
            rhs.LifetimeVariance = node["lifetime_variance"] ? node["lifetime_variance"].as<float>() : 0.5f;

            if (node["position_offset"]) {
                auto offset = node["position_offset"];
                rhs.PositionOffset = glm::vec3(offset[0].as<float>(), offset[1].as<float>(), offset[2].as<float>());
            }

            if (node["initial_velocity"]) {
                auto vel = node["initial_velocity"];
                rhs.InitialVelocity = glm::vec3(vel[0].as<float>(), vel[1].as<float>(), vel[2].as<float>());
            }

            if (node["velocity_variance"]) {
                auto var = node["velocity_variance"];
                rhs.VelocityVariance = glm::vec3(var[0].as<float>(), var[1].as<float>(), var[2].as<float>());
            }

            if (node["gravity"]) {
                auto grav = node["gravity"];
                rhs.Gravity = glm::vec3(grav[0].as<float>(), grav[1].as<float>(), grav[2].as<float>());
            }

            if (node["start_color"]) {
                auto col = node["start_color"];
                rhs.StartColor = glm::vec4(col[0].as<float>(), col[1].as<float>(), col[2].as<float>(), col[3].as<float>());
            }

            if (node["end_color"]) {
                auto col = node["end_color"];
                rhs.EndColor = glm::vec4(col[0].as<float>(), col[1].as<float>(), col[2].as<float>(), col[3].as<float>());
            }

            rhs.ParticleSize = node["particle_size"] ? node["particle_size"].as<float>() : 0.1f;
            rhs.TriggerOnLand = node["trigger_on_land"] ? node["trigger_on_land"].as<bool>() : false;
            rhs.TriggerOnDamage = node["trigger_on_damage"] ? node["trigger_on_damage"].as<bool>() : false;
            rhs.TriggerOnAttack = node["trigger_on_attack"] ? node["trigger_on_attack"].as<bool>() : false;
            rhs.BurstCount = node["burst_count"] ? node["burst_count"].as<int>() : 10;

            return true;
        }
    };
}
