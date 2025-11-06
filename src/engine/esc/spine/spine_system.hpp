#pragma once

#include "spine_component.hpp"
#include "../esc_core.hpp"
#include "../../scene/scene.hpp"
#include "../mesh/mesh_renderer_component.hpp"

class SpineSystem final : public ISystemView<SpineComponent, MeshRendererComponent> {
private:
    using TypeDeltaTime = decltype(std::declval<entt::registry>().view<DeltaTimeComponent>());

    std::unique_ptr<spine::SkeletonRenderer> _skeletonRenderer{};
    std::weak_ptr<Scene> _scene{};
    TypeDeltaTime _deltaTime{};

    float _spineScale = 0.01f;
    float _direction = 1;

public:
    explicit SpineSystem(const TypeView &view, const TypeDeltaTime &deltaTime, const std::weak_ptr<Scene> &scene)
        : ISystemView(view), _scene(scene), _deltaTime(deltaTime) {
        _skeletonRenderer = std::make_unique<spine::SkeletonRenderer>();
    }

    void OnTick() override {
        for (const auto [_, deltaTime] : _deltaTime.each()) {
            for (const auto &[_, component, mesh] : View.each()) {
                if (const auto &animationState = component.Spine->GetAnimationState().lock()) {
                    if (const auto &skeleton = component.Spine->GetSkeleton().lock()) {
                        animationState->update(deltaTime.Delta);
                        animationState->apply(*skeleton);

                        skeleton->updateWorldTransform(spine::Physics_None);

                        // support only one draw call from spine
                        if (const spine::RenderCommand *command = _skeletonRenderer->render(*skeleton)) {

                            std::vector<float> vertices{};
                            std::vector<uint32_t> index{};

                            const int num_command_vertices = command->numVertices;

                            const float *positions = command->positions;
                            const float *uvs = command->uvs;
                            for (int i = 0, j = 0; i < num_command_vertices; i++, j += 2) {
                                vertices.push_back(positions[j + 0] * (_spineScale * _direction)); // skeleton->setScaleX() breaks animation
                                vertices.push_back(positions[j + 1] * _spineScale);
                                vertices.push_back(0); //z

                                vertices.push_back(uvs[j + 0]);
                                vertices.push_back(1 - uvs[j + 1]); // flip y-uv
                            }

                            for (int i = 0; i < command->numIndices; i++) {
                                index.push_back(command->indices[i]);
                            }

                            const auto meshPtr = _scene.lock()->GetMesh(mesh.Guid);
                            meshPtr->UpdateData(vertices, index);
                        }
                    }

                }
            }
        }
    }
};