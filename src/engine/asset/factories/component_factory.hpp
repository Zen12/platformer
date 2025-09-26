#pragma once
#include <memory>

#include "../../components/destroy_with_creator.hpp"
#include "../../components/renderering/particle_emitter.hpp"
#include "../../components/transforms/rect_transform_follower.hpp"
#include "../../components/physics/spine_collider.hpp"
#include "../../ui/button/button_component.hpp"
#include "../../gameplay/ai_controller.hpp"
#include "../../gameplay/grid_prefab_spawner.hpp"
#include "../../gameplay/prefab_spawner.hpp"
#include "../../gameplay/health_component.hpp"
#include "../../gameplay/idle_character.hpp"
#include "../../gameplay/path_mover.hpp"
#include "../../gameplay/game_state/game_state_controller.hpp"
#include "../../gameplay/game_state/team_component.hpp"
#include "../../path_finder/astar_finder.hpp"
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

class PathFinderFactory final : public ComponentFactory<AStarFinderComponent, PathFinderSerialization> {
protected:
    void FillComponent(const std::weak_ptr<AStarFinderComponent> &component, const PathFinderSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            if (const auto scene = _scene.lock()) {
                const auto entity = scene->FindByTag(serialization.gridTag);
                comp->Initialize(entity.lock()->GetComponent<GridComponent>());
            }
        }
    }
};

class GridFactory final : public ComponentFactory<GridComponent, GridSerialization> {
protected:
    void FillComponent(const std::weak_ptr<GridComponent> &component, const GridSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            if (const auto scene = _scene.lock()) {
                comp->SetGrid(serialization.grid);
                comp->SetSpawnOffset(serialization.spawnOffset);
                comp->SetSpawnStep(serialization.spawnStep);
            }
        }
    }
};

class GridPrefabSpawnerFactor final : public ComponentFactory<GridPrefabSpawner, GridPrefabSpawnerSerialization> {
protected:
    void FillComponent(const std::weak_ptr<GridPrefabSpawner> &component, const GridPrefabSpawnerSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            if (const auto scene = _scene.lock()) {
                comp->SetPrefabId(serialization.prefabId);
                comp->SetScene(_scene);
                comp->Spawn(scene->FindByTag(serialization.gridTag).lock()->GetComponent<GridComponent>());
            }
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
                    if (const auto entity = _entity.lock()) {

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

                        const auto rigidBody = entity->GetComponent<Rigidbody2dComponent>();
                        world->AddColliderComponent(rigidBody, component, fixtureDef);
                    }
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

                    world->AddRigidBodyComponent(component, body);
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
                comp->SetSettings(Light2dSettings{
                    .Radius = serialization.Radius,
                    .Segments = serialization.Segments,
                    .Color = serialization.Color,
                    .Offset = serialization.Offset,
                    .MaxAngle = serialization.MaxAngle,
                    .StartAngle = serialization.StartAngle,
                });
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

class PathMoverComponentFactor final : public ComponentFactory<PathMover, PathMoverComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<PathMover> &component, const PathMoverComponentSerialization &serialization) override {

        if (const auto entity = _entity.lock()) {
            if (const auto scene = _scene.lock()) {
                if (const auto comp = component.lock()) {

                    comp->SetTransform(entity->GetComponent<Transform>());
                    comp->SetPoints(serialization.Positions);
                    comp->SetSpeed(serialization.Speed);
                }
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
                    if (const auto entity = _entity.lock()) {
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
                        comp->SetParticles(entity->GetComponent<ParticleEmitterComponent>());
                        comp->SetRenderPipeline(render);
                        comp->SetSpineRenderer(entity->GetComponent<SpineRenderer>());
                    }
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
                comp->SetPathFinder(scene->FindByTag(serialization.PathFinderTag).lock()->GetComponent<AStarFinderComponent>());
                comp->SetTarget(scene->FindByTag(characterSettings.AiTargetTransformTag).lock()->GetComponent<Transform>().lock());
                comp->SetGrid(scene->FindByTag(serialization.GridTag).lock()->GetComponent<GridComponent>());
            }
        }
    }
};

class ShowFpsComponentFactory final : public ComponentFactory<ShowFpsComponent, ShowFpsComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<ShowFpsComponent> &component, [[maybe_unused]] const ShowFpsComponentSerialization &serialization) override {
        if (const auto entity = _entity.lock()) {
            if (const auto comp = component.lock()) {
                comp->SetText(entity->GetComponent<UiTextRendererComponent>());
            }
        }
    }
};

class HealthComponentFactory final : public ComponentFactory<HealthComponent, HealthComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<HealthComponent> &component, const HealthComponentSerialization &serialization) override {
        if (const auto comp = component.lock()) {
            comp->SetMaxHealth(serialization.Health);
            comp->SetHealth(serialization.Health);
            comp->SetScene(_scene.lock());
        }
    }
};

class ParticleEmitterComponentFactory final : public ComponentFactory<ParticleEmitterComponent, ParticleEmitterSerialization> {
protected:
    void FillComponent(const std::weak_ptr<ParticleEmitterComponent> &component, const ParticleEmitterSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto comp = component.lock()) {
                comp->SetMaterial(scene->GetMaterial(serialization.materialGuid));
                comp->SetData({
                    .Count = serialization.count,
                    .Duration = serialization.duration,
                    .StartVelocity = serialization.startVelocity,
                    .EndVelocity = serialization.endVelocity,
                    .StartScale = serialization.startScale,
                    .EndScale = serialization.endScale,
                    .StartColor = serialization.startColor,
                    .EndColor = serialization.endColor}
                );

                if (const auto renderPipeline = scene->GetRenderPipeline().lock()) {
                    renderPipeline->AddRenderer(comp);
                }
            }
        }
    }
};

class SpineColliderComponentFactory final : public ComponentFactory<SpineColliderComponent, SpineColliderSerialization> {
protected:
    void FillComponent(const std::weak_ptr<SpineColliderComponent> &component,
        [[maybe_unused]] const SpineColliderSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto entity = _entity.lock()) {
                if (const auto comp = component.lock()) {
                    comp->SetPhysicsWorld(scene->GetPhysicsWorld());
                    comp->SetRenderer(entity->GetComponent<SpineRenderer>());
                }
            }
        }
    }
};



class RectTransformFollowerFactory final : public ComponentFactory<RectTransformFollower, RectTransformFollowerSerialization> {
protected:
    void FillComponent(const std::weak_ptr<RectTransformFollower> &component,
        const RectTransformFollowerSerialization &serialization) override {
        if (const auto scene = _scene.lock()) {
            if (const auto entity = _entity.lock()) {
                if (const auto comp = component.lock()) {
                    comp->SetTransform(entity->GetComponent<RectTransform>());
                    comp->SetCamera(scene->FindByTag("main_camera").lock()->GetComponent<CameraComponent>().lock());
                    comp->SetOffset(serialization.Offset);

                    if (serialization.UseCreator) {
                        if (const auto creator = entity->GetCreator(); !creator.empty()) {
                            const auto creatorEntity = scene->GetEntityById(creator);
                            if (const auto e = creatorEntity.lock()) {
                                comp->SetFollowTarget(e->GetComponent<Transform>());
                            }
                        }
                    } else {
                        const auto target = scene->GetEntityById(serialization.Target);
                        if (const auto e = target.lock()) {
                            comp->SetFollowTarget(e->GetComponent<Transform>());
                        }
                    }
                }
            }
        }
    }
};

class DestroyWithCreatorComponentFactory final : public ComponentFactory<DestroyWithCreatorComponent, DestroyWithCreatorComponentSerialization> {
protected:
    void FillComponent(const std::weak_ptr<DestroyWithCreatorComponent> &component,
        [[maybe_unused]] const DestroyWithCreatorComponentSerialization &serialization) override {

        if (const auto comp = component.lock()) {
            comp->SetScene(_scene.lock());
        }
    }
};

class IdleCharacterComponentFactor final : public ComponentFactory<IdleCharacter, IdleCharacterSerialization> {
protected:
    void FillComponent(const std::weak_ptr<IdleCharacter> &component,
        [[maybe_unused]] const IdleCharacterSerialization &serialization) override {

        if (const auto entity = _entity.lock()) {
            if (const auto comp = component.lock()) {
                comp->SetSpineRenderer(entity->GetComponent<SpineRenderer>());
                comp->SetAnimation(serialization.IdleAnimation);
            }
        }
    }
};


class GameStateFactory final : public ComponentFactory<GameStateController, GameStateData> {
protected:
    void FillComponent(const std::weak_ptr<GameStateController> &component,
        const GameStateData &serialization) override {

        if (const auto comp = component.lock()) {
            comp->SetLoseSceneId(serialization.LooseScene);
            comp->SetWinSceneId(serialization.WinScene);
            comp->SetScene(_scene);
        }
    }
};

class TeamComponentFactory final : public ComponentFactory<TeamComponent, TeamSerialization> {
protected:
    void FillComponent(const std::weak_ptr<TeamComponent> &component,
        const TeamSerialization &serialization) override
    {
        if (const auto &scene = _scene.lock()) {
            const auto gameStateEntity = scene->FindByTag("game_state"); //temp, need FSM

            if (const auto stateEntity = gameStateEntity.lock()) {
                if (const auto stateController = stateEntity->GetComponent<GameStateController>().lock()) {

                    if (const auto comp = component.lock()) {
                        comp->SetTeamRepository(stateController->GetTeamRepository());
                        comp->SetTeam(static_cast<Team>(serialization.Team));
                    }
                }
            }
        }
    }
};

class UiButtonEffectFactory final : public ComponentFactory<UiButtonEffect, UiButtonEffectSerialization> {
protected:
    void FillComponent(const std::weak_ptr<UiButtonEffect> &component,
        [[maybe_unused]] const UiButtonEffectSerialization &serialization) override
    {
        if (const auto entity = _entity.lock()) {
            if (const auto &comp = component.lock()) {
                comp->SetButton(entity->GetComponent<UiButtonComponent>());
                comp->SetTarget(entity->GetComponent<UiImageRendererComponent>());
            }
        }
    }
};















