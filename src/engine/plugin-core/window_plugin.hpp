#pragma once

#include <glm/gtc/type_ptr.hpp>

#include "plugin.hpp"
#include "entt/entity/registry.hpp"
#include "../system/window.hpp"

namespace Core {
    class WindowComponent final
    {
        public:
            glm::u16vec2 Size{};
    };


    class WindowSystem final : public ISystemView<WindowComponent>
    {
        private:
            std::weak_ptr<Window> _window;
        public:

            WindowSystem(const TypeView &view, const std::weak_ptr<Window> &window):
                ISystemView(view),
                  _window(window){
            }

            void OnTick() override {
                if (const auto &window = _window.lock()) {
                    for (const auto &[_, component] : View.each()) {
                        component.Size = glm::u16vec2(window->GetWidth(), window->GetHeight());
                    }
                }
            }
    };

}

