#pragma once

#include "../../components/entity.hpp"
#include "../mesh/mesh_renderer_component.hpp"
#include "../material.hpp"
#include "spine_data.hpp"



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

    void LookAt(const glm::vec3 &lookAt, const std::string &boneName) const;

    [[nodiscard]] glm::vec3 GetBonePosition(const std::string &boneName) const;
    [[nodiscard]] std::pair<glm::vec3, glm::vec3> GetBoneEndpoints(spine::Bone* bone) const;

    [[nodiscard]] spine::Vector<spine::Bone *>  GetBones() const;

    void Update(const float& deltaTime) override;

    void AppendAnimation(const size_t &index, const std::string &animation, const bool& isLoop) const;

    void SetAnimation(const size_t &index, const std::string &animation, const bool& isLoop, const bool& isReverse) const;

    void SetFaceRight(const bool& isRight);

    void SetTransition(const std::string &anim1, const std::string &anim2, const float &time)const;

    void SetSpine(std::weak_ptr<SpineData> spine);

    void SetSpineScale(const float& scale) noexcept;

    void SetMeshRenderer(std::weak_ptr<MeshRenderer> material) noexcept;

    [[nodiscard]] std::string GetMoveAnimation() const;

    [[nodiscard]] std::string GetJumpAnimation() const;

    [[nodiscard]] std::string GetHitAnimation() const;

    [[nodiscard]] std::string GetIdleAnimation() const;
};