#pragma once
#include "entity.hpp"
#include "mesh_renderer.hpp"
#include "../render/material.hpp"
#include "../render/spine/spine_data.hpp"


class SpineRenderer final : public Component {
private:
    std::weak_ptr<SpineData> _spine{};
    std::weak_ptr<MeshRenderer> _meshRenderer{};
    std::unique_ptr<spine::SkeletonRenderer> _skeletonRenderer{};

public:
    explicit SpineRenderer(const std::weak_ptr<Entity> &entity)
        : Component(entity) {

        _skeletonRenderer = std::make_unique<spine::SkeletonRenderer>();
    }

    void Update() const override {
        if (const auto spine = _spine.lock()) {
            if (const auto skeleton = spine->GetSkeleton().lock()) {
                const float deltaTime = 0.01f;
                skeleton->update(deltaTime);
                spine->GetAnimationState().lock()->update(deltaTime);
                spine->GetAnimationState().lock()->apply(*skeleton);

                // Update the skeleton time (used for physics)
                skeleton->update(deltaTime);

                // Calculate the new pose
                skeleton->updateWorldTransform(spine::Physics_Update);

                spine::RenderCommand *command = _skeletonRenderer->render(*skeleton);
                if (command) {
                    const int num_command_vertices = command->numVertices;

                    std::vector<float> vertices{};
                    std::vector<uint32_t> index{};

                    float *positions = command->positions;
                    float *uvs = command->uvs;
                    for (int i = 0, j = 0; i < num_command_vertices; i++, j += 2) {
                        vertices.push_back(positions[j + 0]);
                        vertices.push_back(positions[j + 1]);
                        vertices.push_back(0); //z

                        vertices.push_back(uvs[j + 0]);
                        vertices.push_back(uvs[j + 1]);
                    }

                    for (int i = 0; i < command->numIndices; i++) {
                        index.push_back(command->indices[i]);
                    }

                    _meshRenderer.lock()->UpdateMesh(vertices, index);
                }
            }
        }
    }

    void SetSpine(std::weak_ptr<SpineData> spine) {
        _spine = std::move(spine);
    }

    void SetMeshRenderer(std::weak_ptr<MeshRenderer> material) noexcept {
        _meshRenderer = std::move(material);
    }
};
