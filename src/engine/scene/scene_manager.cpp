#include "scene_manager.hpp"

void SceneManager::LoadScene(const SceneAsset &scene) {

    std::weak_ptr<Entity> sceneCamera;

    if (const auto assetManager = _assetManager.lock()) {
        for (const auto &entity : scene.Entities)
        {
            const auto newEntity = std::make_shared<Entity>();
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

                        const auto materialAsset = assetManager->LoadAssetByGuid<MaterialAsset>(serialization->MaterialGUID);
                        const auto vertexSource = assetManager->LoadSourceByGuid<std::string>(materialAsset.VertexShaderGuid);
                        const auto fragmentSource = assetManager->LoadSourceByGuid<std::string>(materialAsset.FragmentShaderGuid);
                        const auto shader = std::make_shared<Shader>(vertexSource, fragmentSource);
                        _shaders.push_back(shader);


                        const auto material = std::make_shared<Material>(shader);
                        _materials.push_back(material);

                        uiImage->SetMaterial(material);

                        const auto spriteLoaded = assetManager->LoadSourceByGuid<Sprite>(serialization->SpriteGUID);
                        const auto sprite = std::make_shared<Sprite>(spriteLoaded);
                        _sprites.push_back(sprite);

                        uiImage->SetSprite(sprite);

                        _renderPipeline.lock()->AddRenderer(uiImage);
                    }
                }
                else if (comp->getType() == "ui_text") {

                    if (const auto uiText = newEntity->AddComponent<UiTextRenderer>().lock()) {

                        const auto *serialization = dynamic_cast<UiTextComponentSerialization *>(comp.get());

                        const auto materialAsset = assetManager->LoadAssetByGuid<MaterialAsset>(serialization->MaterialGUID);
                        const auto vertexSource = assetManager->LoadSourceByGuid<std::string>(materialAsset.VertexShaderGuid);
                        const auto fragmentSource = assetManager->LoadSourceByGuid<std::string>(materialAsset.FragmentShaderGuid);
                        const auto shader = std::make_shared<Shader>(vertexSource, fragmentSource);
                        _shaders.push_back(shader);

                        const auto material = std::make_shared<Material>(shader);
                        _materials.push_back(material);

                        //const auto fontFile = assetManager->LoadSourceByGuid<Font>(ASSETS_PATH "resources/fonts/Antonio-Bold.ttf");

                        //std::shared_ptr<Font> font = std::make_shared<Font>(fontFile);
                        //_fonts.push_back(font);

                        //material->SetFont(font);
                        uiText->SetMaterial(material);
                        uiText->SetText(serialization->Text);


                        _renderPipeline.lock()->AddRenderer(uiText);
                    }

                }else if (comp->getType() == "sprite_renderer") {

                    if (const auto spriteRenderer = newEntity->AddComponent<SpriteRenderer>().lock()) {

                        const auto *serialization = dynamic_cast<SpriteRenderComponentSerialization *>(comp.get());

                        const auto materialAsset = assetManager->LoadAssetByGuid<MaterialAsset>(serialization->MaterialGuid);
                        const auto vertexSource = assetManager->LoadSourceByGuid<std::string>(materialAsset.VertexShaderGuid);
                        const auto fragmentSource = assetManager->LoadSourceByGuid<std::string>(materialAsset.FragmentShaderGuid);
                        const auto shader = std::make_shared<Shader>(vertexSource, fragmentSource);
                        _shaders.push_back(shader);

                        const auto material = std::make_shared<Material>(shader);
                        _materials.push_back(material);

                        spriteRenderer->SetMaterial(material);

                        const auto spriteSerialization = assetManager->LoadSourceByGuid<Sprite>(serialization->SpriteGuid);

                        const auto sprite = std::make_shared<Sprite>(spriteSerialization);
                        spriteRenderer->SetSprite(sprite);

                        _sprites.push_back(sprite);

                        _renderPipeline.lock()->AddRenderer(spriteRenderer);
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
    _fonts.clear();
}
