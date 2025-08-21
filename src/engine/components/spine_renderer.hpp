#pragma once

#include "entity.hpp"
#include "mesh_renderer.hpp"
#include "../render/material.hpp"
#include "../render/spine/spine_data.hpp"

#define DEBUG_SPINE_RENDERER 0


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


    void LookAt(const glm::vec3 &lookAt, const std::string &boneName) const {
#ifndef NDEBUG
#if DEBUG_SPINE_RENDERER
        // Draw debug direction
        DebugLines::AddLine(lookAt, lookAt + glm::vec3(0, 1, 0));
#endif
#endif
        // Lock spine instance
        const auto spinePtr = _spine.lock();
        if (!spinePtr)
            return;

        // Lock skeleton
        const auto skeletonPtr = spinePtr->GetSkeleton().lock();
        if (!skeletonPtr)
            return;

        // Find the target bone
        spine::Bone* gun = skeletonPtr->findBone(boneName.c_str());
        if (!gun)
            return;
        const auto position = GetEntity().lock()->GetComponent<Transform>().lock()->GetPosition();

        const auto dir = (position - lookAt) / _spineScale;
        gun->setX(-dir.x * _direction);
        gun->setY(-dir.y); // spine is inverted
    }

    [[nodiscard]] glm::vec3 GetBonePosition(const std::string &boneName) const {
        // Lock skeleton
        const auto spinePtr = _spine.lock();
        if (!spinePtr) {
            std::cerr << "SpineRenderer::GetBonePosition: No spinePtr found" << std::endl;
            return {0, 0, 0};
        }

        const auto skeletonPtr = spinePtr->GetSkeleton().lock();
        if (!skeletonPtr) {
            std::cerr << "SpineRenderer::GetBonePosition: No skeletonPtr found" << std::endl;
            return {0, 0, 0};
        }
        spine::Bone* bone = skeletonPtr->findBone(boneName.c_str());

        const auto position = GetEntity().lock()->GetComponent<Transform>().lock()->GetPosition();

        return {(bone->getWorldX() * _spineScale) * _direction + position.x, bone->getWorldY() * _spineScale + position.y, 0};
    }


    void Update(const float& deltaTime) override {
        if (const auto spine = _spine.lock()) {
            if (const auto skeleton = spine->GetSkeleton().lock()) {

                // Update animation state first
                auto animationState = spine->GetAnimationState().lock();

                animationState->update(deltaTime);
                animationState->apply(*skeleton);

                skeleton->updateWorldTransform(spine::Physics_None);

                std::vector<float> vertices{};
                std::vector<uint32_t> index{};

                // Now render
                spine::RenderCommand *command = _skeletonRenderer->render(*skeleton);

                // support only one draw call from spine
                if (command) {
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

                    command = command->next;
                }
                _meshRenderer.lock()->UpdateMesh(vertices, index);

            }
        }
    }

    void AppendAnimation(const size_t &index, const std::string &animation, const bool& isLoop) const {
        if (const auto spine = _spine.lock()) {
            spine->AppendAnimation(index, animation, isLoop);
        }
    }

    void SetAnimation(const size_t &index, const std::string &animation, const bool& isLoop, const bool& isReverse) const {
        if (const auto spine = _spine.lock()) {
            spine->SetAnimation(index, animation, isLoop, isReverse);
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

    std::string GetMoveAnimation()const {
        if (const auto spine = _spine.lock()) {
            return spine->MoveAnimationName;
        }
        return "";
    }

    std::string GetJumpAnimation()const {
        if (const auto spine = _spine.lock()) {
            return spine->JumpAnimationName;
        }
        return "";
    }

    std::string GetHitAnimation()const {
        if (const auto spine = _spine.lock()) {
            return spine->HitAnimationName;
        }
        return "";
    }

    std::string GetIdleAnimation()const {
        if (const auto spine = _spine.lock()) {
            return spine->IdleAnimationName;
        }
        return "";
    }
};
