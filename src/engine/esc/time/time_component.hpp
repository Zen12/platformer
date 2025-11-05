#pragma once

#include "../esc_core.hpp"
#include "../../system/time.hpp"


class DeltaTimeComponent {
public:
    float Delta = 0.02f;
};

class DeltaTimeSystem final : public ISystemView<DeltaTimeComponent> {
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