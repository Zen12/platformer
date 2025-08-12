#include "scene_manager.hpp"

#include "../gameplay/character_controller.hpp"


void SceneManager::LoadScene(const SceneAsset &scene) const {
    PROFILE_SCOPE("Loading of scene " + scene.Name);

    if (const auto assetManager = _scene->GetAssetManager().lock()) {
        std::vector<std::shared_ptr<Entity>> entities;

        for (const auto &entity : scene.Entities) {
            const auto newEntity = std::make_shared<Entity>();
            newEntity->SetId(entity.Guid);
            newEntity->SetTag(entity.Tag);

            entities.push_back(newEntity);
            _scene->AddEntity(newEntity);
        }

        for (int i = 0; i < scene.Entities.size(); i++) {
            auto &entity = scene.Entities[i];
            const auto& newEntity = entities[i];

            for (const auto &comp : entity.Components)
            {
                if (comp->getType() == "camera") {
                    const auto *cam = dynamic_cast<CameraComponentSerialization *>(comp.get());
                    if (const auto camera = newEntity->AddComponent<CameraComponent>().lock()) {
                        camera->SetCamera(Camera{cam->aspectPower, false, cam->isPerspective, _scene->GetWindow()});
                    }
                }
                else if (comp->getType() == "transform")
                {
                    if (const auto cameraTr = newEntity->AddComponent<Transform>().lock()) {
                        auto *tf = dynamic_cast<TransformComponentSerialization *>(comp.get());
                        const auto position = glm::vec3(tf->position.x, tf->position.y, tf->position.z);
                        const auto rotation = glm::vec3(tf->rotation.x, tf->rotation.y, tf->rotation.z);
                        const auto scale = glm::vec3(tf->scale.x, tf->scale.y, tf->scale.z);
                        cameraTr->SetPosition(position);
                        cameraTr->SetEulerRotation(rotation);
                        cameraTr->SetScale(scale);
                    }
                }
                else if (comp->getType() == "rect_transform") {
                    if (const auto rect = newEntity->AddComponent<RectTransform>().lock()) {
                        rect->SetParent(_scene->GetRoot());
                        const auto *tf = dynamic_cast<RectTransformComponentSerialization *>(comp.get());

                        for (const auto &layout : tf->Layouts)
                        {
                            if (layout.Type == "center_x")
                                rect->AddLayoutOption(std::make_unique<CenterXLayoutOption>());
                            else if (layout.Type == "center_y")
                                rect->AddLayoutOption(std::make_unique<CenterYLayoutOption>());
                            else if (layout.Type == "pixel_width")
                                rect->AddLayoutOption(std::make_unique<PixelWidthLayoutOption>(layout.Value));
                            else if (layout.Type == "pixel_height")
                                rect->AddLayoutOption(std::make_unique<PixelHeightLayoutOption>(layout.Value));
                            else if (layout.Type == "pivot")
                                rect->AddLayoutOption(std::make_unique<PivotLayoutOption>(glm::vec2(layout.X, layout.Y)));
                            else if (layout.Type == "pixel_x")
                                rect->AddLayoutOption(std::make_unique<PixelXLayoutOption>(layout.X, static_cast<AlignmentLayout>(layout.Value)));
                            else if (layout.Type == "pixel_y")
                                rect->AddLayoutOption(std::make_unique<PixelYLayoutOption>(layout.Y, static_cast<AlignmentLayout>(layout.Value)));
                            else
                                std::cerr << "Unknown layout type: " << layout.Type << std::endl;
                        }
                    }
                }
                else if (comp->getType() == "ui_image")
                {
                    if (const auto uiImage = newEntity->AddComponent<UiImageRenderer>().lock()) {

                        const auto *serialization = dynamic_cast<UiImageComponentSerialization *>(comp.get());

                        const auto material = GetMaterial(serialization->MaterialGuid);
                        const auto sprite = GetSprite(serialization->SpriteGuid);

                        uiImage->SetMaterial(material);
                        uiImage->SetSprite(sprite);

                        _scene->GetRenderPipeline().lock()->AddRenderer(uiImage);
                    }
                }
                else if (comp->getType() == "ui_text") {

                    if (const auto uiText = newEntity->AddComponent<UiTextRenderer>().lock()) {

                        const auto *serialization = dynamic_cast<UiTextComponentSerialization *>(comp.get());

                        const auto material = GetMaterial(serialization->MaterialGUID);

                        uiText->SetMaterial(material);
                        uiText->SetText(serialization->Text);


                        _scene->GetRenderPipeline().lock()->AddRenderer(uiText);
                    }

                }else if (comp->getType() == "sprite_renderer") {

                    if (const auto spriteRenderer = newEntity->AddComponent<SpriteRenderer>().lock()) {

                        const auto *serialization = dynamic_cast<SpriteRenderComponentSerialization *>(comp.get());
                        const auto material = GetMaterial(serialization->MaterialGuid);
                        const auto sprite = GetSprite(serialization->SpriteGuid);

                        spriteRenderer->SetMaterial(material);
                        spriteRenderer->SetSprite(sprite);

                        _scene->GetRenderPipeline().lock()->AddRenderer(spriteRenderer);
                    }

                }else if (comp->getType() == "spine_renderer") {

                    if (const auto spineRenderer = newEntity->AddComponent<SpineRenderer>().lock()) {

                        const auto *serialization = dynamic_cast<SpineRenderComponentSerialization *>(comp.get());
                        const auto spineAsset = assetManager->LoadAssetByGuid<SpineAsset>(serialization->SpineGuid);

                        const auto sprite = GetSprite(spineAsset.image);

                        const auto spineData = GetSpineData(serialization->SpineGuid, spineAsset);
                        const auto meshRenderer = newEntity->GetComponent<MeshRenderer>();
                        meshRenderer.lock()->SetSprite(sprite);
                        spineRenderer->SetSpine(spineData);
                        spineRenderer->SetSpineScale(spineAsset.spineScale);
                        spineRenderer->SetMeshRenderer(meshRenderer);
                    }

                }else if (comp->getType() == "box_collider") {
                    if (auto boxCollider = newEntity->AddComponent<BoxCollider2DComponent>().lock()) {
                        const auto *serialization = dynamic_cast<Box2dColliderSerialization *>(comp.get());
                        boxCollider->Init(glm::vec2(serialization->scale.x, serialization->scale.y));
                    }
                }else if (comp->getType() == "rigidbody2d") {
                    if (auto rigidBody = newEntity->AddComponent<Rigidbody2dComponent>().lock()) {
                        const auto *serialization = dynamic_cast<Rigidbody2dSerialization *>(comp.get());
                        rigidBody->SetWorld(_scene->GetPhysicsWorld());
                        rigidBody->Init(serialization->isDynamic);
                    }
                } else if (comp->getType() == "light_2d") {
                    if (const auto light2d = newEntity->AddComponent<Light2dComponent>().lock()) {
                        const auto *serialization = dynamic_cast<Light2dComponentSerialization *>(comp.get());
                        const auto ref = GetEntity(serialization->CenterTransform);
                        if (const auto transform = ref->GetComponent<Transform>().lock()) {
                            light2d->SetCenterTransform(transform);
                        }

                        light2d->SetPhysicsWorld(_scene->GetPhysicsWorld());
                        light2d->SetMeshRenderer(newEntity->GetComponent<MeshRenderer>());
                        light2d->SetOffset(glm::vec3(serialization->OffsetX, serialization->OffsetY, 0.0));
                    }
                }else if (comp->getType() == "mesh_renderer") {
                    if (const auto mesh_renderer = newEntity->AddComponent<MeshRenderer>().lock()) {
                        const auto *serialization = dynamic_cast<MeshRendererComponentSerialization *>(comp.get());
                        mesh_renderer->SetMaterial(GetMaterial(serialization->MaterialGuid));
                        _scene->GetRenderPipeline().lock()->AddRenderer(mesh_renderer);
                    }
                }else if (comp->getType() == "character_controller") {
                    if (const auto component = newEntity->AddComponent<CharacterController>().lock()) {
                        const auto *serialization = dynamic_cast<CharacterControllerComponentSerialization *>(comp.get());
                        const CharacterControllerSettings characterSettings =
                            {
                            serialization->MaxMovementSpeed,
                            serialization->AccelerationSpeed,
                            serialization->DecelerationSpeed,
                            serialization->JumpHeigh,
                            serialization->JumpDuration,
                            serialization->JumpDownMultiplier,
                            serialization->AirControl};
                        component->SetCharacterControllerSettings(characterSettings);
                        component->SetInputSystem(_scene->GetInputSystem());
                        component->SetPhysicsWorld(_scene->GetPhysicsWorld());
                        component->SetSpineRenderer(newEntity->GetComponent<SpineRenderer>());

                    }
                }
            }
        }

        const auto sceneCamera = _scene->FindByTag("main_camera");

        if (const auto camera = sceneCamera.lock()) {
            _scene->GetRenderPipeline().lock()->UpdateCamera(
      camera->GetComponent<CameraComponent>(),
      camera->GetComponent<Transform>());
        }
    }
}

void SceneManager::UnLoadAll() const {
    _scene->UnLoadAll();
}

std::weak_ptr<Entity> SceneManager::GetEntityById(const std::string &id) const {
    return _scene->GetEntityById(id);
}

void SceneManager::Update(const float &deltaTime) const {
    return _scene->Update(deltaTime);
}

std::shared_ptr<Shader> SceneManager::GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) const {
    return _scene->GetShader(vertexGuid, fragmentGuid);
}

std::shared_ptr<Material> SceneManager::GetMaterial(const std::string &guid) const {
    return _scene->GetMaterial(guid);
}

std::shared_ptr<SpineData> SceneManager::LoadSpineData(const SpineAsset& asset) const {
    return _scene->LoadSpineData(asset);
}


std::shared_ptr<SpineData> SceneManager::GetSpineData(const std::string &guid, const SpineAsset& asset) const {
    return _scene->GetSpineData(guid, asset);
}

std::shared_ptr<Sprite> SceneManager::GetSprite(const std::string &guid) const {
    return _scene->GetSprite(guid);
}

std::shared_ptr<Font> SceneManager::GetFont(const std::string &guid) const {
   return _scene->GetFont(guid);
}

std::shared_ptr<Entity> SceneManager::GetEntity(const std::string &id) const {
    return _scene->GetEntity(id);
}
