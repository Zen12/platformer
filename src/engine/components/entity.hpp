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
    explicit Component(const std::weak_ptr<Entity> &entity)
    {
        _entity = entity;
    }

    [[nodiscard]] std::weak_ptr<Entity> GetEntity() const
    {
        return _entity;
    }

    virtual void Update([[maybe_unused]] const float& deltaTime) = 0;
    virtual ~Component() = default;

protected:
    std::weak_ptr<Entity> _entity;
};

class Entity
{
private:
    std::unordered_map<std::type_index, std::shared_ptr<Component>> _components{};
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
        _components[typeid(T)] = component;
        return component;
    }

    template <typename T>
    std::weak_ptr<T> GetComponent() const
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        auto it = _components.find(typeid(T));
        return std::dynamic_pointer_cast<T>(it->second);
    }

    template <typename T>
    void RemoveComponent()
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        _components.erase(typeid(T));
    }

    void Update(const float &deltaTime)
    {
        for (const auto &[type, component] : _components)
        {
            component->Update(deltaTime);
        }
    }

    void SetId(const std::string& id) {
        _id = id;
    }

    void SetTag(const std::string& tag) {
        _tag = tag;
    }

    void SetSelf(const std::shared_ptr<Entity> &self) {
        _self = self;
    }

    [[nodiscard]] const std::string& GetId() const { return _id;}

    [[nodiscard]] const std::string& GetTag() const { return _tag;}

};
