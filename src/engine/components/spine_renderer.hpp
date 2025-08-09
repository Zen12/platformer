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

    float _spineScale = 0.01f;
    float _direction = 1;

public:
    explicit SpineRenderer(const std::weak_ptr<Entity> &entity)
        : Component(entity) {

        _skeletonRenderer = std::make_unique<spine::SkeletonRenderer>();
    }

    void Update(const float& deltaTime) override {
        if (const auto spine = _spine.lock()) {
            if (const auto skeleton = spine->GetSkeleton().lock()) {

                // Update animation state first
                auto animationState = spine->GetAnimationState().lock();

                animationState->update(deltaTime);
                animationState->apply(*skeleton);

                skeleton->updateWorldTransform(spine::Physics_None);

                // Now render
                spine::RenderCommand *command = _skeletonRenderer->render(*skeleton);
                if (command) {
                    const int num_command_vertices = command->numVertices;

                    std::vector<float> vertices{};
                    std::vector<uint32_t> index{};

                    const float *positions = command->positions;
                    const float *uvs = command->uvs;
                    for (int i = 0, j = 0; i < num_command_vertices; i++, j += 2) {
                        vertices.push_back(positions[j + 0] * (_spineScale * _direction));
                        vertices.push_back(positions[j + 1] * _spineScale);
                        vertices.push_back(0); //z

                        vertices.push_back(uvs[j + 0]);
                        vertices.push_back(1 - uvs[j + 1]); // flip y-uv
                    }

                    for (int i = 0; i < command->numIndices; i++) {
                        index.push_back(command->indices[i]);
                    }

                    _meshRenderer.lock()->UpdateMesh(vertices, index);
                }
            }
        }
    }

    void SetAnimation(const std::string &animation) const {
        if (const auto spine = _spine.lock()) {
            spine->SetAnimation(animation);
        }
    }

    void SetFaceRight(const bool& isRight) {
        if (isRight) {
            _direction = 1;
        }else {
            _direction = -1;
        }
    }

    void SetTransition(const std::string &anim1, const std::string &anim2, const float &time)const {
        if (const auto spine = _spine.lock()) {
            spine->SetTransition(anim1, anim2, time);
        }
    }

    void SetSpine(std::weak_ptr<SpineData> spine) {
        _spine = std::move(spine);
    }

    void SetSpineScale(const float& scale) noexcept{
        _spineScale = scale;
    }

    void SetMeshRenderer(std::weak_ptr<MeshRenderer> material) noexcept {
        _meshRenderer = std::move(material);
    }
};
