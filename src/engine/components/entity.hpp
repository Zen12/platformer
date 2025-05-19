#pragma once

#include <type_traits>
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <memory>

class Entity;

class Component
{
    public:
        Component(const std::weak_ptr<Entity>& entity)
        {
            _entity = entity;
        }

        std::weak_ptr<Entity> GetEntity() const
        {
            return _entity;
        }

        virtual void Update() const = 0;
        virtual ~Component() = default;

    protected:
        std::weak_ptr<Entity> _entity;


};

class Entity : public std::enable_shared_from_this<Entity>
{
public:
    Entity() = default;

    template <typename T>
    std::weak_ptr<T> AddComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        auto sharedThis = std::weak_ptr<Entity>(shared_from_this());

        auto component = std::make_shared<T>(sharedThis);
        _components[typeid(T)] = component;
        return component;
    }

    template <typename T>
    std::weak_ptr<T> GetComponent() const
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        auto it = _components.find(typeid(T));
        return std::dynamic_pointer_cast<T>(it->second);
    }

    template <typename T>
    void RemoveComponent()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        _components.erase(typeid(T));
    }

    void Update() const
    {
        for (const auto& [type, component] : _components)
        {
            component->Update();
        }
    }

private:
    std::unordered_map<std::type_index, std::shared_ptr<Component>> _components;
};
