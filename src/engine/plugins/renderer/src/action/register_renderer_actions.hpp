#pragma once

#include "register/fsm_action_registry.hpp"
#include "engine_context.hpp"

#include "start_video_recording_action.hpp"
#include "start_video_recording_action_serialization.hpp"
#include "start_video_recording_action_serialization_yaml.hpp"

#include "stop_video_recording_action.hpp"
#include "stop_video_recording_action_serialization.hpp"
#include "stop_video_recording_action_serialization_yaml.hpp"

inline void RegisterRendererActions(FsmActionRegistry& registry) {
    registry.Register<StartVideoRecordingActionSerialization>("start_video_recording",
        [](const StartVideoRecordingActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<StartVideoRecordingAction>(s.OutputFile, s.Fps, ctx.Get<VideoRecorder>("VideoRecorder"));
        }
    );

    registry.Register<StopVideoRecordingActionSerialization>("stop_video_recording",
        []([[maybe_unused]] const StopVideoRecordingActionSerialization& s, const EngineContext& ctx) {
            return std::make_unique<StopVideoRecordingAction>(ctx.Get<VideoRecorder>("VideoRecorder"));
        }
    );
}
