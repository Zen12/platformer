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
    registry.Register("play_sound",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<PlaySoundActionSerialization>();
            return std::make_unique<PlaySoundActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const PlaySoundActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<PlaySoundAction>(s->AudioGuid, s->Volume, s->Loop, ctx.GetWeak<AudioManager>("AudioManager"));
        }
    );

    registry.Register("play_sound_repeated",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<PlaySoundRepeatedActionSerialization>();
            return std::make_unique<PlaySoundRepeatedActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const PlaySoundRepeatedActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<PlaySoundRepeatedAction>(
                s->AudioGuid, s->Volume, s->DelaySeconds,
                s->Spatial, s->MinDistance, s->MaxDistance,
                ctx.GetWeak<AudioManager>("AudioManager"),
                ctx.GetWeak<FsmAnimationComponent>("FsmAnimationComponent"));
        }
    );

    registry.Register("mute_audio",
        [](const YAML::Node& n) -> std::unique_ptr<ActionSerialization> {
            auto s = n.as<MuteAudioActionSerialization>();
            return std::make_unique<MuteAudioActionSerialization>(std::move(s));
        },
        [](const ActionSerialization* ser, const EngineContext& ctx) -> std::unique_ptr<Action> {
            const auto* s = dynamic_cast<const MuteAudioActionSerialization*>(ser);
            if (!s) return nullptr;
            return std::make_unique<MuteAudioAction>(s->Mute, s->OnlyDebug, ctx.GetWeak<AudioManager>("AudioManager"));
        }
    );
}
