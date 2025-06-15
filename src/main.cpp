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

    std::weak_ptr<Entity> sceneCamera;

    const auto rectRoot = std::make_shared<RectTransformRoot>(window);

    for (const auto &entity : scene.entities)
    {
        const auto newEntity = std::make_shared<Entity>();
        entities.push_back(newEntity);

        for (const auto &comp : entity.components)
        {
            if (comp->getType() == "camera")
            {
                auto *cam = dynamic_cast<CameraComponentSerialization *>(comp.get());
                const auto camera = newEntity->AddComponent<CameraComponent>();
                camera.lock()->SetCamera(Camera{cam->aspectPower, cam->isPerspective, window});
                sceneCamera = newEntity;
            }
            else if (comp->getType() == "transform")
            {
                const auto cameraTr = newEntity->AddComponent<Transform>();

                auto *tf = dynamic_cast<TransformComponentSerialization *>(comp.get());
                const auto position = glm::vec3(tf->position.x, tf->position.y, tf->position.z);
                const auto rotation = glm::vec3(tf->rotation.x, tf->rotation.y, tf->rotation.z);
                const auto scale = glm::vec3(tf->scale.x, tf->scale.y, tf->scale.z);
                cameraTr.lock()->SetPosition(position);
                cameraTr.lock()->SetEulerRotation(rotation);
                cameraTr.lock()->SetScale(scale);
            }
            else if (comp->getType() == "rect_transform") {
                const auto rect = newEntity->AddComponent<RectTransform>();
                rect.lock()->SetParent(rectRoot);

                const auto *tf = dynamic_cast<RectTransformComponentSerialization *>(comp.get());
                for (const auto &layout : tf->Layouts)
                {
                    std::cout << layout.Type << " " << layout.Value << " " << layout.X << " " << layout.Y << std::endl;
                    if (layout.Type == "center_x")
                        rect.lock()->AddLayoutOption(std::make_unique<CenterXLayoutOption>());
                    else if (layout.Type == "center_y")
                        rect.lock()->AddLayoutOption(std::make_unique<CenterYLayoutOption>());
                    else if (layout.Type == "pixel_width")
                        rect.lock()->AddLayoutOption(std::make_unique<PixelWidthLayoutOption>(layout.Value));
                    else if (layout.Type == "pixel_height")
                        rect.lock()->AddLayoutOption(std::make_unique<PixelHeightLayoutOption>(layout.Value));
                    else if (layout.Type == "pivot") {
                        rect.lock()->AddLayoutOption(std::make_unique<PivotLayoutOption>(glm::vec2(layout.X, layout.Y)));
                    } else {
                        std::cerr << "Unknown layout type: " << layout.Type << std::endl;
                    }
                }
            }
            else if (comp->getType() == "ui_image")
            {
                const auto uiImage = newEntity->AddComponent<UiImageRenderer>();

                const auto *serialization = dynamic_cast<UiImageComponentSerialization *>(comp.get());

                const auto materialSerialization = assetManager.LoadMaterialByGuid(serialization->MaterialGUID);

                const auto shader = std::make_shared<Shader>(materialSerialization.Shader.vertexSourceCode, materialSerialization.Shader.fragmentShourceCode);
                shaders.push_back(shader);

                const auto material = std::make_shared<Material>(shader);
                materials.push_back(material);

                uiImage.lock()->SetMaterial(material);

                const auto spriteSerialization = assetManager.LoadSpriteByGuid(serialization->SpriteGUID);

                const auto sprite = std::make_shared<Sprite>(spriteSerialization.Path);
                sprites.push_back(sprite);

                uiImage.lock()->SetSprite(sprite);

                renderPipeline.AddRenderer(uiImage);
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
