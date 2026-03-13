#pragma once

#ifndef __EMSCRIPTEN__

#include "core/src/plugin/plugin_registry.hpp"
#include "esc/esc_system_context.hpp"
#include "src/terminal_system.hpp"

struct TerminalPlugin {
    static void Register(PluginRegistries& registries) {
        registries.Systems.Register<TerminalSystem>("TerminalSystem",
            [](const EscSystemContext& ctx) {
                auto scene = ctx.Scene.lock();
                if (!scene) return std::unique_ptr<TerminalSystem>(nullptr);
                return std::make_unique<TerminalSystem>(ctx.Scene, ctx.ProjectRoot);
            }, 950);
    }
};

#endif
