#pragma once

#include "esc/esc_core.hpp"
#include "time.hpp"


class DeltaTimeComponent {
public:
    float Delta = 0.02f;
    float FixedDelta = 0.02f;
};

class DeltaTimeSystem final : public ISystemView<DeltaTimeComponent> {
    Time _deltaTime{};

public:
    explicit DeltaTimeSystem(const TypeView &view) : ISystemView(view) {
        _deltaTime.Start();
    }

    void OnTick() override {
        const auto deltaTimeValue = _deltaTime.GetResetDelta();
        for (const auto &[_, deltaTime]: View.each()) {
            deltaTime.Delta = deltaTimeValue;
            deltaTime.FixedDelta = 0.02f;
        }
        _deltaTime.Reset();
    }

};
