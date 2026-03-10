#pragma once
#include <memory>
#include "entt/entt.hpp"

class Scene;
class RenderRepository;
class ResourceCache;
class AudioManager;

struct EscSystemContext {
    std::shared_ptr<entt::registry> Registry;
    std::weak_ptr<Scene> Scene;
    std::shared_ptr<RenderRepository> RenderRepository;
    std::shared_ptr<ResourceCache> ResourceCache;
    std::weak_ptr<AudioManager> AudioManager;
};
