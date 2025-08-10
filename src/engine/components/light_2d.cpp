#include "light_2d.hpp"

void Light2dComponent::Update([[maybe_unused]] const float &deltaTime) {
    if (const auto world = _physicsWorld.lock()->GetWorld().lock()) {
        if (const auto center = _center.lock()) {
            std::vector<float> meshVert{};
            std::vector<unsigned int> meshIndex{};

            const auto centerPosition = center->GetPosition();

            meshVert.emplace_back(centerPosition.x + _offset.x);
            meshVert.emplace_back(centerPosition.y + _offset.y);
            meshVert.emplace_back(0);

            // uv
            meshVert.emplace_back(0);
            meshVert.emplace_back(0);

            const float step = 360.0f / static_cast<float>(_segments);

            for (int i = 0; i < _segments; i++) {
                const float angle = static_cast<float>(i) * step;

                const float angleRad = DegToRad(angle);
                float x = centerPosition.x + _radius * std::cos(angleRad);
                float y = centerPosition.y + _radius * std::sin(angleRad);

                b2Vec2 pointA(centerPosition.x, centerPosition.y);    // Start of ray
                b2Vec2 pointB(x, y);   // End of ray

                RayCastClosestCallback callback;
                world->RayCast(&callback, pointA, pointB);

                if (callback.Hit) {
                    x = callback.Point.x;
                    y = callback.Point.y;
                }

                const auto vertex = std::vector<float>{
                    centerPosition.x, centerPosition.y, 0.0f,
                    x,y, 0.0f};

                meshVert.emplace_back(x);
                meshVert.emplace_back(y);
                meshVert.emplace_back(0);
                // uv
                meshVert.emplace_back(0);
                meshVert.emplace_back(0);

            }

            for (int j = 1; j < _segments; j++) {
                meshIndex.emplace_back(0);
                meshIndex.emplace_back(j);
                meshIndex.emplace_back(j+1);
            }

            meshIndex.emplace_back(0);
            meshIndex.emplace_back(_segments);
            meshIndex.emplace_back(1);

            if (const auto meshRenderer = _meshRenderer.lock()) {
                meshRenderer->SetUniformVec3("center", centerPosition);
                meshRenderer->UpdateMesh(meshVert, meshIndex);
            }
        }
    }
}
