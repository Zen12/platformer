#pragma once
#include "box2d/b2_math.h"
#include "box2d/b2_world_callbacks.h"

class RayCastClosestCallback : public b2RayCastCallback {
public:
    b2Vec2 point;
    b2Vec2 normal;
    float fraction = 1.0f;
    bool hit = false;

    float ReportFixture(b2Fixture* fixture, const b2Vec2& p,
                        const b2Vec2& n, float f) override {
        hit = true;
        point = p;
        normal = n;
        fraction = f;
        return f; // return the fraction to clip the ray and get closest hit
    }
};
