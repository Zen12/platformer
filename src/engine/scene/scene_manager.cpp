#include "scene_manager.hpp"


void SceneManager::LoadScene(const SceneAsset &scene) {
    PROFILE_SCOPE("Loading of scene " + scene.Name);

    if (const auto assetManager = _assetManager.lock()) {
        std::weak_ptr<Entity> sceneCamera;
        for (const auto &entity : scene.Entities)
        {
            const auto newEntity = std::make_shared<Entity>();
            newEntity->SetId(entity.guid);

            _entities.push_back(newEntity);

            for (const auto &comp : entity.components)
            {
                if (comp->getType() == "camera") {
                    const auto *cam = dynamic_cast<CameraComponentSerialization *>(comp.get());
                    if (const auto camera = newEntity->AddComponent<CameraComponent>().lock()) {
                        camera->SetCamera(Camera{cam->aspectPower, false, cam->isPerspective, _window});
                        sceneCamera = newEntity;
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
                        rect->SetParent(_root);
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

                        const auto material = GetMaterial(serialization->MaterialGUID);
                        const auto sprite = GetSprite(serialization->SpriteGUID);

                        uiImage->SetMaterial(material);
                        uiImage->SetSprite(sprite);

                        _renderPipeline.lock()->AddRenderer(uiImage);
                    }
                }
                else if (comp->getType() == "ui_text") {

                    if (const auto uiText = newEntity->AddComponent<UiTextRenderer>().lock()) {

                        const auto *serialization = dynamic_cast<UiTextComponentSerialization *>(comp.get());

                        const auto material = GetMaterial(serialization->MaterialGUID);

                        uiText->SetMaterial(material);
                        uiText->SetText(serialization->Text);


                        _renderPipeline.lock()->AddRenderer(uiText);
                    }

                }else if (comp->getType() == "sprite_renderer") {

                    if (const auto spriteRenderer = newEntity->AddComponent<SpriteRenderer>().lock()) {

                        const auto *serialization = dynamic_cast<SpriteRenderComponentSerialization *>(comp.get());
                        const auto material = GetMaterial(serialization->MaterialGuid);
                        const auto sprite = GetSprite(serialization->SpriteGuid);

                        spriteRenderer->SetMaterial(material);
                        spriteRenderer->SetSprite(sprite);

                        _renderPipeline.lock()->AddRenderer(spriteRenderer);
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
                        rigidBody->SetWorld(_physicsWorld);
                        rigidBody->Init(serialization->isDynamic);
                    }
                }else if (comp->getType() == "line_renderer") {
                    if (auto lineRenderer = newEntity->AddComponent<LineRenderer>().lock()) {
                        const auto *serialization = dynamic_cast<LineRenderComponentSerialization *>(comp.get());
                        lineRenderer->SetMaterial(GetMaterial(serialization->MaterialGuid));
                        _renderPipeline.lock()->AddRenderer(lineRenderer);
                    }
                }else if (comp->getType() == "light_2d") {
                    if (const auto light2d = newEntity->AddComponent<Light2dComponent>().lock()) {
                        const auto *serialization = dynamic_cast<Light2dComponentSerialization *>(comp.get());
                        const auto ref = GetEntity(serialization->CenterTransform);
                        if (const auto transform = ref->GetComponent<Transform>().lock()) {
                            light2d->SetCenterTransform(transform);
                        }

                        light2d->SetPhysicsWorld(_physicsWorld);
                        light2d->SetMeshRenderer(newEntity->GetComponent<MeshRenderer>());
                    }
                }else if (comp->getType() == "mesh_renderer") {
                    if (const auto mesh_renderer = newEntity->AddComponent<MeshRenderer>().lock()) {
                        const auto *serialization = dynamic_cast<MeshRendererComponentSerialization *>(comp.get());
                        mesh_renderer->SetMaterial(GetMaterial(serialization->MaterialGuid));
                        _renderPipeline.lock()->AddRenderer(mesh_renderer);
                    }
                }
            }
        }

        _renderPipeline.lock()->UpdateCamera(
            sceneCamera.lock()->GetComponent<CameraComponent>(),
            sceneCamera.lock()->GetComponent<Transform>());
    }
}

void SceneManager::UnLoadAll() {
    _entities.clear();
    _shaders.clear();
    _materials.clear();
    _sprites.clear();
    _spineDatas.clear();
    _fonts.clear();
    _meshes.clear();
}

std::shared_ptr<Shader> SceneManager::GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) {
    if (const auto assetManager = _assetManager.lock()) {

        if (_shaders.find(vertexGuid + fragmentGuid) == _shaders.end()) {
            const auto vertexSource = assetManager->LoadSourceByGuid<std::string>(vertexGuid);
            const auto fragmentSource = assetManager->LoadSourceByGuid<std::string>(fragmentGuid);
            const auto shader = std::make_shared<Shader>(vertexSource, fragmentSource);
            _shaders[vertexGuid + fragmentGuid] = shader;
            return shader;
        }

        return _shaders[vertexGuid + fragmentGuid];
    }

    return {};
}

std::shared_ptr<Material> SceneManager::GetMaterial(const std::string &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (_materials.find(guid) == _materials.end()) {
            const auto materialAsset = assetManager->LoadAssetByGuid<MaterialAsset>(guid);
            const auto shader = GetShader(materialAsset.VertexShaderGuid, materialAsset.FragmentShaderGuid);
            const auto material = std::make_shared<Material>(shader);

            const auto font = GetFont(materialAsset.Font);
            material->SetFont(font);

            _materials[guid] = material;
            return material;
        }
        return _materials[guid];
    }

    return {};
}

std::shared_ptr<SpineData> SceneManager::LoadSpineData( const SpineAsset& asset) const {
    if (const auto assetManager = _assetManager.lock()) {
        // Load the atlas and the skeleton data
        const auto atlas = assetManager->LoadSourceByGuid<spine::Atlas*>(asset.atlas);
        spine::SkeletonBinary binary(atlas);

        const auto skeletonPath = assetManager->GetPathFromGuid(asset.skeleton);
        spine::SkeletonData *skeletonData = binary.readSkeletonDataFile(skeletonPath.c_str());

        std::shared_ptr<spine::Skeleton> skeleton = std::make_shared<spine::Skeleton>(skeletonData);

        spine::AnimationStateData animationStateData(skeletonData);

        return std::make_shared<SpineData>(skeleton, std::make_shared<spine::AnimationState>(&animationStateData));
    }

    return {};
}


std::shared_ptr<SpineData> SceneManager::GetSpineData(const std::string &guid, const SpineAsset& asset) {

    if (const auto assetManager = _assetManager.lock()) {
        if (_spineDatas.find(guid) == _spineDatas.end()) {
            const auto spineData = LoadSpineData(asset);
            _spineDatas[guid] = spineData;
            return spineData;
        }
        return _spineDatas[guid];
    }
    return {};
}

std::shared_ptr<Sprite> SceneManager::GetSprite(const std::string &guid) {
    if (const auto assetManager = _assetManager.lock()) {
        if (_sprites.find(guid) == _sprites.end()) {
            const auto sprite = std::make_shared<Sprite>(assetManager->LoadSourceByGuid<Sprite>(guid));
            _sprites[guid] = sprite;
            return sprite;
        }
        return _sprites[guid];
    }
    return {};
}

std::shared_ptr<Font> SceneManager::GetFont(const std::string &guid) {
    if (guid.empty())
        return {};

    if (const auto assetManager = _assetManager.lock()) {
        if (_fonts.find(guid) == _fonts.end()) {
            const auto fontFile = assetManager->LoadSourceByGuid<Font>(guid);
            std::shared_ptr<Font> font = std::make_shared<Font>(fontFile);
            _fonts[guid] = font;
            return font;
        }
        return _fonts[guid];
    }
    return {};
}

std::shared_ptr<Entity> SceneManager::GetEntity(const std::string &id) const {
    for (const auto & entity: _entities) {
        if (entity->GetId() == id)
            return entity;
    }
    return {};
}
