#pragma once
#include <cstdint>

enum class BTNodeType : uint8_t {
    // Composite nodes
    Sequence = 0,       // Runs children in order, fails on first failure
    Selector = 1,       // Runs children in order, succeeds on first success
    Parallel = 2,       // Runs all children simultaneously

    // Decorator nodes
    Inverter = 10,      // Inverts child result
    Repeater = 11,      // Repeats child N times
    UntilFail = 12,     // Repeats child until it fails

    // Condition nodes (leaf)
    CheckDistance = 20, // Check distance to target
    HasTarget = 21,     // Check if entity has a target
    CheckHealth = 22,   // Check health threshold
    IsMoving = 23,      // Check if currently moving

    // Action nodes (leaf)
    MoveTo = 30,        // Move to target position
    MoveToTarget = 31,  // Move to current target entity
    Attack = 32,        // Attack current target
    Idle = 33,          // Do nothing
    Wait = 34,          // Wait for duration
    RandomWander = 35,  // Pick random point and move there
    SetTarget = 36,     // Set target from perception

    // Target finding nodes
    FindTargetByTag = 40,   // Find nearest entity with tag (param: tag hash)
    HasTargetInRange = 41,  // Check if target is within range (param: distance)
    ClearTarget = 42,       // Clear current target
};

enum class BTStatus : uint8_t {
    Success = 0,
    Failure = 1,
    Running = 2,
};
