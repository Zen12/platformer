#pragma once

#include <type_traits>
#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <memory>

class Entity;

struct ComponentSerialization
{
    virtual ~ComponentSerialization() = default;
    [[nodiscard]] virtual std::string getType() const = 0;
};

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

class Entity : public std::enable_shared_from_this<Entity>
{
private:
    std::unordered_map<std::type_index, std::shared_ptr<Component>> _components{};
    std::string _id{};
    std::string _tag{};

public:
    Entity() = default;

    template <typename T>
    std::weak_ptr<T> AddComponent()
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

        auto sharedThis = std::weak_ptr<Entity>(shared_from_this());

        auto component = std::make_shared<T>(sharedThis);
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

    [[nodiscard]] const std::string& GetId() const { return _id;}

    [[nodiscard]] const std::string& GetTag() const { return _tag;}

};

/*
template<class TComponentSerialization>
class ComponentFactory {
    static_assert(std::is_base_of_v<ComponentSerialization, TComponentSerialization>,
                  "TComponentSerialization must derive from ComponentSerialization");

protected:
    std::weak_ptr<Scene> _scene;


public:
    explicit ComponentFactory(std::weak_ptr<Scene> scene)
        : _scene(std::move(scene))
    {}
    virtual ~ComponentFactory() = default;

    template <typename TComponent>
    void Create(const TComponentSerialization& serialization, Entity e) {
        static_assert(std::is_base_of_v<Component, TComponent>,
                      "TComponent must derive from Component");

        const auto componentPtr = e.AddComponent<TComponent>();
        if (auto component = componentPtr.lock()) {
            SetComponentImpl(serialization, component.get());
        }
    }

protected:
    // Non-templated virtual: erased to base type
    virtual void SetComponentImpl(const TComponentSerialization& serialization,
                                  Component* component) = 0;
};
*/