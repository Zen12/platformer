#pragma once

#include "core/src/register/esc_system_registry.hpp"
#include "esc/esc_system_context.hpp"

#include "audio_source/audio_system.hpp"

inline void RegisterAudioSystems(EscSystemRegistry& registry) {
    // AudioSystem: priority 280
    registry.Register("AudioSystem", [](const EscSystemContext& ctx) -> std::unique_ptr<ISystem> {
        const auto reg = ctx.Registry;
        return std::make_unique<AudioSystem>(
            reg->view<AudioSourceComponent, TransformComponentV2>(),
            reg->view<AudioListenerComponent, TransformComponentV2>(),
            ctx.AudioManager);
    }, 280);
}
