#pragma once
#include <memory>

class NavigationManager;

struct NavigationManagerComponent {
    std::shared_ptr<NavigationManager> Manager;
};
