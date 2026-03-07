#pragma once

#include "entity/component_registry.hpp"

#include "audio_source/audio_source_component.hpp"
#include "audio_source/audio_source_component_serialization.hpp"
#include "audio_source/audio_source_component_serialization_yaml.hpp"

#include "audio_listener/audio_listener_component.hpp"
#include "audio_listener/audio_listener_component_serialization.hpp"
#include "audio_listener/audio_listener_component_serialization_yaml.hpp"

inline void RegisterAudioComponents(ComponentRegistry& registry) {
    registry.Register<AudioSourceComponentSerialization, AudioSourceComponent>("audio_source",
        [](const AudioSourceComponentSerialization& s) {
            return AudioSourceComponent(
                s.AudioClipGuid,
                s.Volume,
                s.Pitch,
                s.Loop,
                s.AutoPlay,
                s.Spatial,
                s.MinDistance,
                s.MaxDistance
            );
        }
    );

    registry.Register<AudioListenerComponentSerialization, AudioListenerComponent>("audio_listener",
        []([[maybe_unused]] const AudioListenerComponentSerialization& s) {
            return AudioListenerComponent();
        }
    );
}
