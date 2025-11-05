#pragma once

#include "entt/entt.hpp"


class ISystem {
public:
    virtual void OnTick() = 0;
    virtual ~ISystem() = default;
};

template <typename T>
class ISystemView : public ISystem {
protected:
    using TypeView = decltype(std::declval<entt::registry>().view<T>());
    const TypeView View{};

public:
    explicit ISystemView(const TypeView& view)
        :View(view) {}

    void OnTick() override = 0;
};



