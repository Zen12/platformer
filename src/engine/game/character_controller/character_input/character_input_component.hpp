#pragma once
#include "../../../entity/entity.hpp"
#include "../../../system/input_system.hpp"
#include "../../../renderer/render_pipeline.hpp"
#include <glm/vec3.hpp>

struct CharacterInputData {
    glm::vec3 MovementDirection = glm::vec3(0);
    glm::vec3 LookAtPosition = glm::vec3(0);
    bool Jump = false;
    bool Shoot = false;
};

class CharacterInputComponent final : public Component {
private:
    std::weak_ptr<InputSystem> _inputSystem;
    std::weak_ptr<RenderPipeline> _renderPipeline;

    [[nodiscard]] glm::vec3 GetMousePosition() const;

public:
    explicit CharacterInputComponent(const std::weak_ptr<Entity> &entity)
        : Component(entity) {
    }

    void SetInputSystem(const std::weak_ptr<InputSystem> &inputSystem) noexcept {
        _inputSystem = inputSystem;
    }

    void SetRenderPipeline(const std::weak_ptr<RenderPipeline> &renderPipeline) noexcept {
        _renderPipeline = renderPipeline;
    }

    [[nodiscard]] CharacterInputData GetInputData() const;

    void Update(const float &deltaTime) override;
};
