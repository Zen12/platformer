#pragma once

class HealthComponent final {
private:
    float _maxHealth = 100.0f;
    float _currentHealth = 100.0f;
    bool _isDead = false;

public:
    bool JustTookDamage = false;  // Set for one frame when damage is taken

    HealthComponent() = default;

    explicit HealthComponent(float maxHealth)
        : _maxHealth(maxHealth), _currentHealth(maxHealth), _isDead(false) {}

    [[nodiscard]] float GetMaxHealth() const noexcept { return _maxHealth; }
    void SetMaxHealth(float health) noexcept { _maxHealth = health; }

    [[nodiscard]] float GetCurrentHealth() const noexcept { return _currentHealth; }
    void SetCurrentHealth(float health) noexcept { _currentHealth = health; }

    [[nodiscard]] bool IsDead() const noexcept { return _isDead; }
    void SetDead(bool dead) noexcept { _isDead = dead; }

    void TakeDamage(float damage) {
        _currentHealth -= damage;
        if (_currentHealth <= 0.0f) {
            _currentHealth = 0.0f;
            _isDead = true;
        }
    }

    void Heal(float amount) {
        _currentHealth += amount;
        if (_currentHealth > _maxHealth) {
            _currentHealth = _maxHealth;
        }
    }
};
