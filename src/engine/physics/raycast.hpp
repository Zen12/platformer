#pragma once
#include "box2d/b2_math.h"
#include "box2d/b2_world_callbacks.h"

class RayCastClosestCallback final : public b2RayCastCallback {
public:
    b2Vec2 Point;
    b2Vec2 Normal;
    float Fraction = 1.0f;
    bool Hit = false;
    b2Fixture *Fixture;

    float ReportFixture(b2Fixture* fixture, const b2Vec2& p,
                        const b2Vec2& n, const float f) override {
        Hit = true;
        Point = p;
        Normal = n;
        Fraction = f;
        Fixture = fixture;
        return f; // return the fraction to clip the ray and get closest hit
    }
};
