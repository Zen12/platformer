#pragma once
#include "../../../plugin-core/plugin.hpp"
#include "../../../system/time.hpp"
#include "entt/entity/registry.hpp"

namespace Plugins::Common {

    class DeltaTimeComponent {
    public:
        float Delta = 0.02f;
    };

    class DeltaTimeSystem final : public Core::ISystemView<DeltaTimeComponent> {
        Time _deltaTime{};

    public:
        explicit DeltaTimeSystem(const TypeView &view) : ISystemView(view) {
            _deltaTime.Start();
        }

        void OnTick() override {
            const auto deltaTimeValue = _deltaTime.GetResetDeltaFps();
            for (const auto &[_, deltaTime]: View.each()) {
                deltaTime.Delta = deltaTimeValue;
            }
            _deltaTime.Reset();
        }

    };
}




