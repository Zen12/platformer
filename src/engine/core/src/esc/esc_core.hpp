#pragma once

#include "entt/entt.hpp"


class ISystem {
public:
    virtual void OnTick() = 0;
    virtual ~ISystem() = default;
};

template <typename... Components>
class ISystemView : public ISystem {
protected:
    using TypeView = decltype(std::declval<entt::registry&>().view<Components...>());
    TypeView View;

public:
    explicit ISystemView(TypeView view) : View(view) {}
    ~ISystemView() override = default;

    void OnTick() override = 0;
};



