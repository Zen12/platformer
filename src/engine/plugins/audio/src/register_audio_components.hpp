#pragma once

#include "entity/component_registry.hpp"

#include "esc/audio_source/audio_source_component.hpp"
#include "esc/audio_source/audio_source_component_serialization.hpp"
#include "esc/audio_source/audio_source_component_serialization_yaml.hpp"

#include "esc/audio_listener/audio_listener_component.hpp"
#include "esc/audio_listener/audio_listener_component_serialization.hpp"
#include "esc/audio_listener/audio_listener_component_serialization_yaml.hpp"

inline void RegisterAudioComponents(ComponentRegistry& registry) {
    registry.Register("audio_source",
        [](const YAML::Node& n) -> std::unique_ptr<ComponentSerialization> {
            auto s = n.as<AudioSourceComponentSerialization>();
            return std::make_unique<AudioSourceComponentSerialization>(std::move(s));
        },
        [](entt::registry& reg, entt::entity entity, const ComponentSerialization* ser) {
            const auto* s = dynamic_cast<const AudioSourceComponentSerialization*>(ser);
            if (!s) return;
            const auto& view = reg.view<AudioSourceComponent>();
            AudioSourceComponent audioComp(
                s->AudioClipGuid,
                s->Volume,
                s->Pitch,
                s->Loop,
                s->AutoPlay,
                s->Spatial,
                s->MinDistance,
                s->MaxDistance
            );
            view->emplace(entity, audioComp);
        }
    );

    registry.Register("audio_listener",
        [](const YAML::Node& n) -> std::unique_ptr<ComponentSerialization> {
            auto s = n.as<AudioListenerComponentSerialization>();
            return std::make_unique<AudioListenerComponentSerialization>(std::move(s));
        },
        [](entt::registry& reg, entt::entity entity, [[maybe_unused]] const ComponentSerialization* ser) {
            const auto& view = reg.view<AudioListenerComponent>();
            view->emplace(entity, AudioListenerComponent());
        }
    );
}
