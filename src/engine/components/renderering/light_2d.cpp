#include "light_2d.hpp"

#ifndef NDEBUG
#include "../../debug/debug.hpp"
#define LIGHT_COMPONENT_DEBUG 1
#define ENGINE_DEBUG_LIGHT_PROFILE 0
#endif

void Light2dComponent::Update([[maybe_unused]] const float &deltaTime) {
    if (const auto world = _physicsWorld.lock()) {
        if (const auto center = _center.lock()) {
            std::vector<float> meshVert{};
            std::vector<unsigned int> meshIndex{};

            const auto isEnclose = _settings.StartAngle < 0.1f && _settings.MaxAngle >= 359;

            const auto centerPosition = center->GetPosition() + _settings.Offset;

            meshVert.emplace_back(centerPosition.x);
            meshVert.emplace_back(centerPosition.y);
            meshVert.emplace_back(centerPosition.z);

            // uv
            meshVert.emplace_back(0);
            meshVert.emplace_back(0);


            const float step = _settings.MaxAngle / static_cast<float>(_settings.Segments);

            for (int i = 0; i < _settings.Segments; i++) {
                const float angle = static_cast<float>(i) * step;

                const float angleRad = DegToRad(angle + _settings.StartAngle);
                float x = centerPosition.x + _settings.Radius * std::cos(angleRad);
                float y = centerPosition.y + _settings.Radius * std::sin(angleRad);
                float uvX = 0;
                float uvY = 0;

#if ENGINE_DEBUG_LIGHT_PROFILE
                ScopedTimer *scoped = new ScopedTimer("     Light2dComponent::Update::Raycast");
#endif
                const auto result = world->RayCast(centerPosition, glm::vec3(x, y, 0));
#if ENGINE_DEBUG_LIGHT_PROFILE
                delete scoped;
#endif

                if (result.IsHit) {
                    x = result.Point.x;
                    y = result.Point.y;

                    //const auto fixture = world->GetFixtureByCollider(result.BoxCollider);

                    //const auto poly = dynamic_cast<b2PolygonShape*>(fixture->GetShape());

                    //int nextRay = FindNexClosesVertex(fixture, poly, glm::vec2(centerPosition.x, centerPosition.y), glm::vec2(x, y));

                    //if (nextRay > poly->m_count) {
                      //  nextRay = 0;
                    //}

                   // const auto nextVertPos = poly->m_vertices[nextRay];

                    //float angleToNewVert = atan2(centerPosition.y, centerPosition.x) - atan2(nextVertPos.y, nextVertPos.x);

                    //if (angleRad < angleToNewVert) {
                    //    i+=10;
                    //}

                    uvX = 1;
                    uvY = 1;
                }

                meshVert.emplace_back(x);
                meshVert.emplace_back(y);
                meshVert.emplace_back(centerPosition.z);
                // uv
                meshVert.emplace_back(uvX);
                meshVert.emplace_back(uvY);

            }


            for (int j = 1; j < _settings.Segments; j++) {
                meshIndex.emplace_back(0);
                meshIndex.emplace_back(j);
                meshIndex.emplace_back(j+1);
            }

            if (isEnclose) {
                meshIndex.emplace_back(0);
                meshIndex.emplace_back(_settings.Segments);
                meshIndex.emplace_back(1);
            }

            if (const auto meshRenderer = _meshRenderer.lock()) {
                meshRenderer->GetEntity().lock()->GetComponent<Transform>().lock()->SetPosition(centerPosition);
                meshRenderer->UpdateMesh(meshVert, meshIndex);
                meshRenderer->SetUniformVec3("center", centerPosition);
                meshRenderer->SetUniformVec3("color", _settings.Color);
                meshRenderer->SetUniformFloat("radius", _settings.Radius);
#ifndef NDEBUG
#if LIGHT_COMPONENT_DEBUG
                meshRenderer->DrawDebug();
#endif
#endif
            }
        }
    }
}
