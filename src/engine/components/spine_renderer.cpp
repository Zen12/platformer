#include "spine_renderer.hpp"


void SpineRenderer::SetMaterial(const std::weak_ptr<Material> &material) noexcept {
    _material = material;
}

void SpineRenderer::Update() const {
    if (const auto material = _material.lock()) {
        material->Bind();
        if (const auto spine = _spine.lock()) {
            spine->Bind();
        }
    }
}

void SpineRenderer::Render() const noexcept {
    if (const auto material = _material.lock()) {
        if (const auto spine = _spine.lock()) {
            glDrawElements(GL_TRIANGLES, static_cast<int32_t>(spine->GetIndicesCount()), GL_UNSIGNED_INT, nullptr);
        }
    }
}