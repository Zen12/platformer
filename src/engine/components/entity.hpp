#pragma once

#include <type_traits>
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <vector>
#include <algorithm>

#include "../debug/debug.hpp"

#define DEBUG_ENGINE_ENTITY_PROFILE 0

class Entity;

class Component
{
public:
    explicit Component(const std::weak_ptr<Entity>& entity)
        : _entity(entity) {}

    [[nodiscard]] std::weak_ptr<Entity> GetEntity() const { return _entity; }

    virtual void Update([[maybe_unused]] const float& deltaTime) = 0;
    virtual ~Component() = default;

protected:
    std::weak_ptr<Entity> _entity;
};

class Entity : public std::enable_shared_from_this<Entity>
{
private:
    std::unordered_map<std::type_index, std::vector<std::shared_ptr<Component>>> _components{};
    std::string _id{};
    std::string _tag{};
    std::weak_ptr<Entity> _self{};

public:
    Entity() = default;

    template <typename T>
    std::weak_ptr<T> AddComponent()
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        auto component = std::make_shared<T>(_self);
        _components[typeid(T)].push_back(component);
        return component;
    }

    // Return first found (like Unity)
    template <typename T>
    std::weak_ptr<T> GetComponent() const
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        auto it = _components.find(typeid(T));
        if (it != _components.end() && !it->second.empty()) {
            return std::dynamic_pointer_cast<T>(it->second.front());
        }
        return {};
    }

    // Return all components of type T
    template <typename T>
    std::vector<std::weak_ptr<T>> GetComponents() const
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        std::vector<std::weak_ptr<T>> result;
        auto it = _components.find(typeid(T));
        if (it != _components.end()) {
            for (const auto& comp : it->second) {
                result.push_back(std::dynamic_pointer_cast<T>(comp));
            }
        }
        return result;
    }

    template <typename T>
    void RemoveComponents()
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        _components.erase(typeid(T));
    }

    template <typename T>
    void RemoveComponent(const std::weak_ptr<T>& componentToRemove)
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        auto it = _components.find(typeid(T));
        if (it != _components.end()) {
            auto& vec = it->second;
            vec.erase(std::remove_if(vec.begin(), vec.end(),
                                     [&](const std::shared_ptr<Component>& comp) {
                                         return comp == componentToRemove.lock();
                                     }),
                      vec.end());
            if (vec.empty()) {
                _components.erase(it);
            }
        }
    }

    void Update(const float& deltaTime)
    {
        for (const auto& [type, comps] : _components) {
            for (const auto& comp : comps) {
#ifndef NDEBUG
#if DEBUG_ENGINE_ENTITY_PROFILE
                std::string typeName = type.name();  // may be mangled
                PROFILE_SCOPE("   "  + _self.lock()->GetId() + " " + typeName);
#endif
#endif
                comp->Update(deltaTime);
            }
        }
    }

    void SetId(const std::string& id) { _id = id; }
    void SetTag(const std::string& tag) { _tag = tag; }
    void SetSelf(const std::shared_ptr<Entity>& self) { _self = self; }

    [[nodiscard]] const std::string& GetId() const { return _id; }
    [[nodiscard]] const std::string& GetTag() const { return _tag; }
};
