#pragma once

struct CombatStateComponent final {
    bool IsAttacking = false;
    float AttackTimer = 0.0f;
};
