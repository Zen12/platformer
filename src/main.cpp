#include <iostream>
#include "engine/engine.hpp"

int main()
{
    const std::string projectRoot = ASSETS_PATH;
    const ProjectAsset projectAsset = AssetLoader::LoadProjectAssetFromPath(projectRoot + "project.yaml");

    AssetManager assetManager(projectRoot);
    assetManager.Init();

    auto window = std::make_shared<Window>(800, 600, projectAsset.Name);
    window->WinInit();

    RenderPipeline renderPipeline = {window};

    const auto scene = assetManager.LoadSceneByGuid(projectAsset.Scenes[0]);

    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<Shader>> shaders;
    std::vector<std::shared_ptr<Material>> materials;
    std::vector<std::shared_ptr<Sprite>> sprites;
    std::vector<std::shared_ptr<Font>> fonts;

    std::weak_ptr<Entity> sceneCamera;
    const auto rectRoot = std::make_shared<RectTransformRoot>(window);

    for (const auto &entity : scene.entities)
    {
        const auto newEntity = std::make_shared<Entity>();
        entities.push_back(newEntity);

        for (const auto &comp : entity.components)
        {
            if (comp->getType() == "camera") {
                const auto *cam = dynamic_cast<CameraComponentSerialization *>(comp.get());
                if (const auto camera = newEntity->AddComponent<CameraComponent>().lock()) {
                    camera->SetCamera(Camera{cam->aspectPower, cam->isPerspective, window});
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
                    rect->SetParent(rectRoot);
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
                    const auto materialSerialization = assetManager.LoadMaterialByGuid(serialization->MaterialGUID);

                    const auto shader = std::make_shared<Shader>(materialSerialization.Shader.vertexSourceCode, materialSerialization.Shader.fragmentShourceCode);
                    shaders.push_back(shader);

                    const auto material = std::make_shared<Material>(shader);
                    materials.push_back(material);

                    uiImage->SetMaterial(material);

                    const auto spriteSerialization = assetManager.LoadSpriteByGuid(serialization->SpriteGUID);

                    const auto sprite = std::make_shared<Sprite>(spriteSerialization.Path);
                    sprites.push_back(sprite);

                    uiImage->SetSprite(sprite);

                    renderPipeline.AddRenderer(uiImage);
                }
            }
            else if (comp->getType() == "ui_text") {

                if (const auto uiText = newEntity->AddComponent<UiTextRenderer>().lock()) {

                    const auto *serialization = dynamic_cast<UiTextComponentSerialization *>(comp.get());
                    const auto materialSerialization = assetManager.LoadMaterialByGuid(serialization->MaterialGUID);

                    const auto shader = std::make_shared<Shader>(materialSerialization.Shader.vertexSourceCode, materialSerialization.Shader.fragmentShourceCode);
                    shaders.push_back(shader);

                    const auto material = std::make_shared<Material>(shader);
                    materials.push_back(material);

                    std::shared_ptr<Font> font = std::make_shared<Font>(AssetLoader::LoadFontFromPath(ASSETS_PATH "resources/fonts/Antonio-Bold.ttf"));
                    fonts.push_back(font);

                    material->SetFont(font);
                    uiText->SetMaterial(material);
                    uiText->SetText(serialization->Text);


                    renderPipeline.AddRenderer(uiText);
                }

            }
        }
    }

    renderPipeline.UpdateCamera(
        sceneCamera.lock()->GetComponent<CameraComponent>(),
        sceneCamera.lock()->GetComponent<Transform>());

    Time time;

    time.Start();

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    while (window->IsOpen())
    {
        time.Reset();

        glClear(GL_COLOR_BUFFER_BIT);

        renderPipeline.RenderSprites();
        renderPipeline.RenderUI();

        window->SwapBuffers();
        glfwPollEvents();
    }

    window->Destroy();
}
