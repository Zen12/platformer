#pragma once
#include <memory>
#include <string>
#include "entt/entt.hpp"

class Scene;
class ResourceFactory;
class AudioManager;

struct EscSystemContext {
    std::shared_ptr<entt::registry> Registry;
    std::weak_ptr<Scene> Scene;
    std::shared_ptr<ResourceFactory> ResourceFactory;
    std::weak_ptr<AudioManager> AudioManager;
    std::string ProjectRoot;
};
