#pragma once

#include "../../../core/src/register/fsm_action_registry.hpp"
#include "engine_context.hpp"

#include "play_sound_action.hpp"
#include "play_sound_action_serialization.hpp"
#include "play_sound_action_serialization_yaml.hpp"

#include "play_sound_repeated_action.hpp"
#include "play_sound_repeated_action_serialization.hpp"
#include "play_sound_repeated_action_serialization_yaml.hpp"

#include "mute_audio_action.hpp"
#include "mute_audio_action_serialization.hpp"
#include "mute_audio_action_serialization_yaml.hpp"

inline void RegisterAudioActions(FsmActionRegistry& registry) {
    registry.Register<PlaySoundActionSerialization>("play_sound",
        [](const PlaySoundActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<PlaySoundAction>(s.AudioGuid, s.Volume, s.Loop, ctx.GetWeak<AudioManager>("AudioManager"));
        }
    );

    registry.Register<PlaySoundRepeatedActionSerialization>("play_sound_repeated",
        [](const PlaySoundRepeatedActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<PlaySoundRepeatedAction>(
                s.AudioGuid, s.Volume, s.DelaySeconds,
                s.Spatial, s.MinDistance, s.MaxDistance,
                ctx.GetWeak<AudioManager>("AudioManager"),
                ctx.GetWeak<FsmAnimationComponent>("FsmAnimationComponent"));
        }
    );

    registry.Register<MuteAudioActionSerialization>("mute_audio",
        [](const MuteAudioActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<MuteAudioAction>(s.Mute, s.OnlyDebug, ctx.GetWeak<AudioManager>("AudioManager"));
        }
    );
}
