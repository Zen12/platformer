#include "spine_renderer.hpp"

#include "../../debug/debug.hpp"
#define DEBUG_SPINE_RENDERER 0

void SpineRenderer::LookAt(const glm::vec3 &lookAt, const std::string &boneName) const {
#if DEBUG_SPINE_RENDERER
        // Draw debug direction
        DebugLines::AddLine(lookAt, lookAt + glm::vec3(0, 1, 0));
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

glm::vec3 SpineRenderer::GetBonePosition(const std::string &boneName) const {
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

std::pair<glm::vec3, glm::vec3> SpineRenderer::GetBoneEndpoints(spine::Bone* bone) const {
    if (!bone) {
        std::cerr << "SpineRenderer::GetBoneEndpoints: Bone is null" << std::endl;
        return {{0, 0, 0}, {0, 0, 0}};
    }

    const auto entityTransform = GetEntity().lock()->GetComponent<Transform>().lock();
    const glm::vec3 position = entityTransform ? entityTransform->GetPosition() : glm::vec3(0.0f);

    // Start point (joint position)
    const glm::vec3 start{
        (bone->getWorldX() * _spineScale) * _direction + position.x,
        bone->getWorldY() * _spineScale + position.y,
        0.0f
    };

    // End point (tip of the bone)
    const float angleRad = glm::radians(bone->getWorldRotationX());
    const float length   = bone->getData().getLength() * bone->getWorldScaleX() * _spineScale;

    const glm::vec3 end{
        start.x + std::cos(angleRad) * length * _direction,
        start.y + std::sin(angleRad) * length,
        0.0f
    };

    return {start, end};
}

spine::Vector<spine::Bone *> SpineRenderer::GetBones() const {
    if (const auto skeletonPtr = _spine.lock()) {
        if (const auto skeleton = skeletonPtr->GetSkeleton().lock()) {
            return skeleton->getBones();
        }
    }
    return {};
}

void SpineRenderer::Update(const float &deltaTime) {
    if (const auto spine = _spine.lock()) {
        if (const auto skeleton = spine->GetSkeleton().lock()) {

            // Update animation state first
            auto animationState = spine->GetAnimationState().lock();

            animationState->update(deltaTime);
            animationState->apply(*skeleton);

            skeleton->updateWorldTransform(spine::Physics_None);


            std::vector<float> vertices{};
            std::vector<uint32_t> index{};

            // support only one draw call from spine
            if (const spine::RenderCommand *command = _skeletonRenderer->render(*skeleton)) {
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

                if (const auto renderer = _meshRenderer.lock()) {
                    renderer->UpdateMesh(vertices, index);
                }
            }
        }
    }
}

void SpineRenderer::AppendAnimation(const size_t &index, const std::string &animation, const bool &isLoop) const {
    if (const auto spine = _spine.lock()) {
        spine->AppendAnimation(index, animation, isLoop);
    }
}

void SpineRenderer::SetAnimation(const size_t &index, const std::string &animation, const bool &isLoop,
    const bool &isReverse) const {
    if (const auto spine = _spine.lock()) {
        spine->SetAnimation(index, animation, isLoop, isReverse);
    }
}

void SpineRenderer::SetFaceRight(const bool &isRight) {
    if (isRight) {
        _direction = 1;
    }else {
        _direction = -1;
    }
}

void SpineRenderer::SetTransition(const std::string &anim1, const std::string &anim2, const float &time) const {
    if (const auto spine = _spine.lock()) {
        spine->SetTransition(anim1, anim2, time);
    }
}

void SpineRenderer::SetSpine(std::weak_ptr<SpineData> spine) {
    _spine = std::move(spine);
}

void SpineRenderer::SetSpineScale(const float &scale) noexcept {
    _spineScale = scale;
}

void SpineRenderer::SetMeshRenderer(std::weak_ptr<MeshRenderer> material) noexcept {
    _meshRenderer = std::move(material);
}

std::string SpineRenderer::GetMoveAnimation() const {
    if (const auto spine = _spine.lock()) {
        return spine->MoveAnimationName;
    }
    return "";
}

std::string SpineRenderer::GetJumpAnimation() const {
    if (const auto spine = _spine.lock()) {
        return spine->JumpAnimationName;
    }
    return "";
}

std::string SpineRenderer::GetHitAnimation() const {
    if (const auto spine = _spine.lock()) {
        return spine->HitAnimationName;
    }
    return "";
}

std::string SpineRenderer::GetIdleAnimation() const {
    if (const auto spine = _spine.lock()) {
        return spine->IdleAnimationName;
    }
    return "";
}
