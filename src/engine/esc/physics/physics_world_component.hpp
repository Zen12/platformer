#pragma once
#include <memory>
#include <unordered_map>

#include "box2d/b2_fixture.h"
#include "box2d/b2_world.h"

struct RayCastSimpleResult final {
    bool IsHit{};
    glm::vec3 Point{};
    glm::vec3 Normal{};
    uint32_t BoxColliderId{};
};


class PhysicsWorldComponent final {
private:
    RayCastSimpleResult _result{};
    RayCastClosestCallback _callback{};

    std::unique_ptr<b2World> World{};

    std::unordered_map<uint32_t, std::unique_ptr<b2FixtureDef>> _fixtures{};
    std::unordered_map<uint32_t, std::unique_ptr<b2PolygonShape>> _shapes{};

    uint32_t _currentShapeIndex = 0;

public:

    [[nodiscard]] uint32_t Add(std::unique_ptr<b2FixtureDef> fixture, std::unique_ptr<b2PolygonShape> shape) {
        _currentShapeIndex++;

        b2FixtureUserData data{};
        data.pointer = _currentShapeIndex;
        fixture->userData = data;
        _fixtures[_currentShapeIndex] = std::move(fixture);
        _shapes[_currentShapeIndex] = std::move(shape);

        return _currentShapeIndex;
    }

    void UpdateShape(const uint32_t &id, const glm::vec2& position, const glm::vec2& scale) {

        auto shape = std::move( _shapes[id]);
        shape->SetAsBox(
            scale.x / 2,             // half width
            scale.y / 2,             // half height
            b2Vec2(position.x, position.y), // offset upward by half its height
            0.0f                                   // rotation
        );

        _shapes[id] = std::move(shape);
    }

    [[nodiscard]] const RayCastSimpleResult& Raycast(const glm::vec3& origin, const glm::vec3& target) {
        const b2Vec2 pointA(origin.x, origin.y);    // Start of ray
        const b2Vec2 pointB(target.x, target.y);   // End of ray

        _result.IsHit = false;
        _result.BoxColliderId = 0;

        World->RayCast(&_callback, pointA, pointB);

        _result.IsHit = _callback.Hit;

        if (_callback.Hit) {
            _result.Point = glm::vec3(_callback.Point.x, _callback.Point.y, 0.0f);
            _result.Normal = glm::vec3(_callback.Normal.x, _callback.Normal.y, 0.0f);
            _result.BoxColliderId = _callback.Fixture->GetUserData().pointer;
        }

        return _result;
    }

};

