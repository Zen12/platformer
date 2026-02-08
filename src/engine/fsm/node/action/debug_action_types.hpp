#pragma once
#include "fps_display_action.hpp"
#include "health_display_action.hpp"
#include "health_bar_action.hpp"
#include "start_video_recording_action.hpp"
#include "stop_video_recording_action.hpp"
#include "../../type_list.hpp"

using DebugActionTypes = fsm::TypeList<
    FpsDisplayAction,
    HealthDisplayAction,
    HealthBarAction,
    StartVideoRecordingAction,
    StopVideoRecordingAction
>;
