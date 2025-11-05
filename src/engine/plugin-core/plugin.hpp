#pragma once
#include "../renderer/transform/transform_component_serialization.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Core {

    class ISystem {
        public:
            virtual void OnTick() = 0;
            virtual ~ISystem() = default;
    };

    template <typename T>
    class ISystemView : public ISystem{
    protected:
        using TypeView = decltype(std::declval<entt::registry>().view<T>());
        const TypeView View{};

    public:
        explicit ISystemView(const TypeView& view)
            :View(view) {}

        void OnTick() override = 0;
    };


    class TransformComponentV2 final {
    private:
        glm::vec3 _position = glm::vec3(0);
        glm::vec3 _eulerRotation = glm::vec3(0);
        glm::vec3 _scale = glm::vec3(1);
    public:
        explicit TransformComponentV2(const TransformComponentSerialization& serialization) :
            _position(serialization.position), _eulerRotation(serialization.rotation), _scale(serialization.scale)
        {}

        [[nodiscard]] glm::mat4 GetModel() const {
            auto model = glm::scale(glm::mat4(1.0f), _scale);

            model = glm::rotate(model, glm::radians(_eulerRotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate around X
            model = glm::rotate(model, glm::radians(_eulerRotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y
            model = glm::rotate(model, glm::radians(_eulerRotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Z

            model = glm::translate(model, _position);

            return model;
        }
    };

    class TagComponent {
    private:
        const std::string _tag{};
    public:
        explicit TagComponent(std::string tag)
            : _tag(std::move(tag))
        {}

        [[nodiscard]] const std::string &GetTag() const noexcept { return _tag; }
    };

}
