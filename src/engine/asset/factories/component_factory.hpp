#pragma once
#include <memory>

#include "../../gameplay/ai_controller.hpp"
#include "../../gameplay/grid_prefab_spawner.hpp"
#include "../../gameplay/prefab_spawner.hpp"
#include "../../gameplay/health_component.hpp"
#include "../../scene/scene.hpp"

class BaseComponentFactory {
public:
    virtual ~BaseComponentFactory() = default;
};

template<typename TComponent, typename TSerialization>
class ComponentFactory : BaseComponentFactory {

static_assert(std::is_base_of_v<ComponentSerialization, TSerialization>,
              "TSerialization must inherit from ComponentSerialization");

static_assert(std::is_base_of_v<Component, TComponent>,
          "TSerialization must inherit from ComponentSerialization");

protected:
    std::weak_ptr<Scene> _scene;
    std::weak_ptr<Entity> _entity;
    virtual void FillComponent(const std::weak_ptr<TComponent> &component, const TSerialization &serialization) = 0;

public:

    void SetScene(const std::weak_ptr<Scene> &scene) { _scene = scene; }
    void SetEntity(const std::weak_ptr<Entity> &entity) { _entity = entity; }

    void AddComponent(const std::weak_ptr<Entity>& entity, const TSerialization &serialization) {
        if (const auto e = entity.lock()) {
            const auto component = e->AddComponent<TComponent>();
            FillComponent(component, serialization);
        }
    }
};


class CameraComponentFactory final : public ComponentFactory<CameraComponent, CameraComponentSerialization> {

protected:
    void FillComponent(const std::weak_ptr<CameraComponent> &component, const CameraComponentSerialization &serialization)  override {
        if (const auto camera = component.lock()) {
            if (const auto scene = _scene.lock()) {
                camera->SetCamera
                        (Camera{serialization.aspectPower, false, serialization.isPerspective, _scene.lock()->GetWindow()});
                camera->SetWindow(scene->GetWindow());
            }
        }
    }

};


class TransformFactory final : public ComponentFactory<Transform, TransformComponentSerialization> {
    protected:
    void FillComponent(const std::weak_ptr<Transform> &component, const TransformComponentSerialization &serialization) override {
        if (const auto transform = component.lock()) {
            transform->SetPosition(serialization.position);
            transform->SetEulerRotation(serialization.rotation);
            transform->SetScale(serialization.scale);
        }

    }
};

class GridPrefabSpawnerFactor final : public ComponentFactory<GridPrefabSpawner, GridPrefabSpawnerSerialization> {
protected:
    void FillComponent(const std::weak_ptr<GridPrefabSpawner> &component, const GridPrefabSpawnerSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            comp->SetPrefabId(serialization.prefabId);
            comp->SetScene(_scene);
            comp->SetSpawnOffset(serialization.spawnOffset);
            comp->SetSpawnStep(serialization.spawnStep);
            comp->Spawn(serialization.grid);
        }
    }
};

class PrefabSpawnerFactor final : public ComponentFactory<PrefabSpawner, PrefabSpawnerSerialization> {
protected:
    void FillComponent(const std::weak_ptr<PrefabSpawner> &component, const PrefabSpawnerSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            comp->SetPrefabId(serialization.prefabId);
            comp->SetScene(_scene);
            comp->SetMaxCount(serialization.maxSpawn);
            comp->SetSpawnTimer(serialization.spawnTime);
            comp->SetPrefabPositions(serialization.positions);
        }
    }
};

class RectTransformFactory final : public ComponentFactory<RectTransform, RectTransformComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<RectTransform> &component, const RectTransformComponentSerialization &serialization) override {

        if (const auto rect = component.lock()) {
            rect->SetParent(_scene.lock()->GetRoot());

            for (const auto &layout : serialization.Layouts)
            {
                if (layout.Type == "center_x")
                    rect->AddLayoutOption<CenterXLayoutOption>();
                else if (layout.Type == "center_y")
                    rect->AddLayoutOption<CenterYLayoutOption>();
                else if (layout.Type == "pixel_width")
                    rect->AddLayoutOption<PixelWidthLayoutOption>(layout.Value);
                else if (layout.Type == "pixel_height")
                    rect->AddLayoutOption<PixelHeightLayoutOption>(layout.Value);
                else if (layout.Type == "pivot")
                    rect->AddLayoutOption<PivotLayoutOption>(glm::vec2(layout.X, layout.Y));
                else if (layout.Type == "pixel_x")
                    rect->AddLayoutOption<PixelXLayoutOption>(layout.X, static_cast<AlignmentLayout>(layout.Value));
                else if (layout.Type == "pixel_y")
                    rect->AddLayoutOption<PixelYLayoutOption>(layout.Y, static_cast<AlignmentLayout>(layout.Value));
                else
                    std::cerr << "Unknown layout type: " << layout.Type << std::endl;
            }
        }
    }
};

class UiImageFactory final : public ComponentFactory<UiImageRenderer, UiImageComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<UiImageRenderer> &component, const UiImageComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto uiImage = component.lock()) {
                const auto material = scene->GetMaterial(serialization.MaterialGuid);
                const auto sprite = scene->GetSprite(serialization.SpriteGuid);

                uiImage->SetMaterial(material);
                uiImage->SetSprite(sprite);

                scene->GetRenderPipeline().lock()->AddRenderer(component);
            }
        }
    }
};

class UiTextComponentFactory final : public ComponentFactory<UiTextRenderer, UiTextComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<UiTextRenderer> &component, const UiTextComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                const auto material = scene->GetMaterial(serialization.MaterialGUID);

                comp->SetMaterial(material);
                comp->SetText(serialization.Text);

                scene->GetRenderPipeline().lock()->AddRenderer(comp);
            }
        }
    }
};

class SpriteRendererFactory final : public ComponentFactory<SpriteRenderer, SpriteRenderComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<SpriteRenderer> &component, const SpriteRenderComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                const auto material = scene->GetMaterial(serialization.MaterialGuid);
                const auto sprite = scene->GetSprite(serialization.SpriteGuid);

                comp->SetMaterial(material);
                comp->SetSprite(sprite);

                scene->GetRenderPipeline().lock()->AddRenderer(component);
            }
        }
    }
};


class SpineRendererFactory final : public ComponentFactory<SpineRenderer, SpineRenderComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<SpineRenderer> &component, const SpineRenderComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                const auto spineAsset = scene->GetAssetManager().lock()->LoadAssetByGuid<SpineAsset>(serialization.SpineGuid);

                const auto sprite = scene->GetSprite(spineAsset.image);

                const auto spineData = scene->GetSpineData(serialization.SpineGuid, spineAsset);
                const auto meshRenderer = _entity.lock()->GetComponent<MeshRenderer>();
                meshRenderer.lock()->SetSprite(sprite);
                comp->SetSpine(spineData);
                comp->SetSpineScale(spineAsset.spineScale);
                comp->SetMeshRenderer(meshRenderer);
            }
        }
    }
};

class BoxCollider2dFactory final : public ComponentFactory<BoxCollider2DComponent, Box2dColliderSerialization> {
protected:
    void FillComponent(const std::weak_ptr<BoxCollider2DComponent> &component, const Box2dColliderSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                if (const auto world = scene->GetPhysicsWorld().lock()) {
                    b2PolygonShape dynamicBox;
                    // Half extents, then offset, then angle
                    dynamicBox.SetAsBox(
                        serialization.scale.x / 2,             // half width
                        serialization.scale.y / 2,             // half height
                        b2Vec2(serialization.translate.x, serialization.translate.y), // offset upward by half its height
                        0.0f                                   // rotation
                    );

                    b2FixtureDef fixtureDef;
                    fixtureDef.shape = &dynamicBox;
                    fixtureDef.density = 1.0f; // TODO move to serialization
                    fixtureDef.friction = 0.3f;  // TODO move to serialization

                    const auto rigidBody = _entity.lock()->GetComponent<Rigidbody2dComponent>();
                    const auto collider = _entity.lock()->AddComponent<BoxCollider2DComponent>();
                    world->AddColliderComponent(rigidBody, collider, fixtureDef);

                }
            }
        }
    }
};

class Rigidbody2dFactory final : public ComponentFactory<Rigidbody2dComponent, Rigidbody2dSerialization> {
protected:
    void FillComponent(const std::weak_ptr<Rigidbody2dComponent> &component, const Rigidbody2dSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                if (const auto& world = scene->GetPhysicsWorld().lock()) {

                    auto def = b2BodyDef();
                    if (serialization.isDynamic)
                        def.type = b2_dynamicBody;
                    else
                        def.type = b2_staticBody;

                    const auto position = _entity.lock()->GetComponent<Transform>().lock()->GetPosition();
                    def.position.Set(position.x, position.y);

                    b2Body *body = world->GetWorld().lock()->CreateBody(&def);

                    b2MassData massData;
                    massData.mass = 5.0f;  // TODO move to serialization
                    massData.center = body->GetLocalCenter();  // usually (0,0)
                    massData.I = 1.0f;  // moment of inertia, set appropriately
                    body->SetMassData(&massData);

                    body->SetLinearDamping(1.0f);

                    std::weak_ptr<Rigidbody2dComponent> rigidComponent = _entity.lock()->AddComponent<Rigidbody2dComponent>();
                    world->AddRigidBodyComponent(rigidComponent, body);
                }
            }
        }
    }
};

class Light2dFactory final : public ComponentFactory<Light2dComponent, Light2dComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<Light2dComponent> &component, const Light2dComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                const auto ref = scene->GetEntity(serialization.CenterTransform);
                if (const auto transform = ref->GetComponent<Transform>().lock()) {
                    comp->SetCenterTransform(transform);
                }

                comp->SetPhysicsWorld(scene->GetPhysicsWorld());
                comp->SetMeshRenderer(_entity.lock()->GetComponent<MeshRenderer>());
                comp->SetOffset(serialization.Offset);
            }
        }
    }
};

class MeshRendererFactory final : public ComponentFactory<MeshRenderer, MeshRendererComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<MeshRenderer> &component, const MeshRendererComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                comp->SetMaterial(scene->GetMaterial(serialization.MaterialGuid));
                scene->GetRenderPipeline().lock()->AddRenderer(component);
            }
        }
    }
};

class CharacterControllerFactory final : public ComponentFactory<CharacterController, CharacterControllerComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<CharacterController> &component, const CharacterControllerComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                if (const auto render = scene->GetRenderPipeline().lock()) {

                    const CharacterControllerSettings characterSettings =
                        {
                        serialization.MaxMovementSpeed,
                        serialization.AccelerationSpeed,
                        serialization.DecelerationSpeed,
                        serialization.JumpHeigh,
                        serialization.JumpDuration,
                        serialization.JumpDownMultiplier,
                        serialization.AirControl,
                        serialization.Damage,
                        std::string{}};

                    comp->SetCharacterControllerSettings(characterSettings);
                    comp->SetInputSystem(scene->GetInputSystem());
                    comp->SetPhysicsWorld(scene->GetPhysicsWorld());
                    comp->SetRenderPipeline(render);
                    comp->SetSpineRenderer(_entity.lock()->GetComponent<SpineRenderer>());
                }
            }
        }
    }
};

class AiControllerFactory final : public ComponentFactory<AiController, AiControllerComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<AiController> &component, const AiControllerComponentSerialization &serialization) override {

        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {

                const CharacterControllerSettings characterSettings =
                    {
                    serialization.MaxMovementSpeed,
                    serialization.AccelerationSpeed,
                    serialization.DecelerationSpeed,
                    serialization.JumpHeigh,
                    serialization.JumpDuration,
                    serialization.JumpDownMultiplier,
                    serialization.AirControl,
                    serialization.Damage,
                    serialization.AiTargetTransformTag};

                comp->SetCharacterControllerSettings(characterSettings);
                comp->SetInputSystem(scene->GetInputSystem());
                comp->SetPhysicsWorld(scene->GetPhysicsWorld());
                comp->SetSpineRenderer(_entity.lock()->GetComponent<SpineRenderer>());
                comp->SetTarget(scene->GetEntityById(characterSettings.AiTargetTransformTag).lock()->GetComponent<Transform>().lock());

            }
        }
    }
};

class ShowFpsComponentFactory final : public ComponentFactory<ShowFpsComponent, ShowFpsComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<ShowFpsComponent> &component, [[maybe_unused]] const ShowFpsComponentSerialization &serialization) override {
        if (const auto entity = _entity.lock()) {
            if (const auto comp = component.lock()) {
                comp->SetText(entity->GetComponent<UiTextRenderer>());
            }
        }
    }
};

class HealthComponentFactory final : public ComponentFactory<HealthComponent, HealthComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<HealthComponent> &component, [[maybe_unused]] const HealthComponentSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            comp->SetHealth(serialization.Health);
            comp->SetScene(_scene.lock());
        }
    }
};













