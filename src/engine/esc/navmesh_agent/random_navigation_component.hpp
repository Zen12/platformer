#pragma once

class RandomNavigationComponent final {
public:
    float MinX = -20.0f;
    float MaxX = 20.0f;
    float MinZ = -20.0f;
    float MaxZ = 20.0f;
    float WaitTime = 0.0f;
    float CurrentWaitTime = 0.0f;
    int CurrentWaypointIndex = -1;  // -1 means not initialized yet
    bool ReachedDestination = false;

    RandomNavigationComponent() = default;
};
