#pragma once
#include "entity.hpp"
#include "../render/material.hpp"
#include "../render/spine/spine_data.hpp"


class SpineRenderer final : public Component {
private:
    std::weak_ptr<SpineData> _spine{};
    std::weak_ptr<Material> _material{};
    std::unique_ptr<spine::SkeletonRenderer> _skeletonRenderer{};
    Mesh _mesh;

public:
    explicit SpineRenderer(const std::weak_ptr<Entity> &entity)
        : Component(entity), _mesh(Mesh::GenerateSprite()) {

        _skeletonRenderer = std::make_unique<spine::SkeletonRenderer>();
    }

    void Update() const override;

    void SetSpine(const std::weak_ptr<SpineData> &spine) {
        _spine = spine;
    }

    void SetMaterial(const std::weak_ptr<Material> &material) noexcept;

    [[nodiscard]] int32_t GetShaderId() const noexcept {
        if (auto material = _material.lock())
            return material->GetShaderId();

        return -1;
    }


    void Render() noexcept;
};
