#pragma once

class HealthComponent {
private:
    const float _maxHealth{0};

    float _health{0};

public:

    HealthComponent(const float &maxHealth, const float &health)
    : _maxHealth(maxHealth){
        _health = health;
    }

    [[nodiscard]] const float &GetHealth() const noexcept {
        return _health;
    }

    [[nodiscard]] float GetPercentageHealth() const noexcept {
        if (_health <= 0)
            return 0.0f;

        if (_maxHealth <= 0.0f)
            return 0.0f;

        return _health / _maxHealth;
    }

    void DecreaseHealth(const float &health) {
        _health -= health;
    }
};