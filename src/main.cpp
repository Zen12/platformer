#include <iostream>
#include "engine/engine.hpp"

int main()
{
    AssetManager assetManager(ASSETS_PATH);
    assetManager.Load();
    const auto projectAsset = AssetLoader::LoadProjectAssetFromPath(ASSETS_PATH "project.yaml");

    // system init
    auto window = std::make_shared<Window>(800, 600, projectAsset.Name);
    window->WinInit();

    std::shared_ptr<Shader> uiShader = std::make_shared<Shader>(AssetLoader::LoadShaderFromPath(ASSETS_PATH "resources/shaders/uiImage.vert", ASSETS_PATH "resources/shaders/uiImage.frag"));
    std::shared_ptr<Shader> uiText = std::make_shared<Shader>(AssetLoader::LoadShaderFromPath(ASSETS_PATH "resources/shaders/text.vert", ASSETS_PATH "resources/shaders/text.frag"));
    auto texture_face = AssetLoader::LoadSpriteFromPath(ASSETS_PATH "resources/images/awesomeface.png");

    std::shared_ptr<Material> materialUI = std::make_shared<Material>(uiShader);
    std::shared_ptr<Material> materialText = std::make_shared<Material>(uiText);

    std::shared_ptr<Font> font = std::make_shared<Font>(AssetLoader::LoadFontFromPath(ASSETS_PATH "resources/fonts/Antonio-Bold.ttf"));

    materialText->SetFont(font);

    const auto rectRoot = std::make_shared<RectTransformRoot>(window);

    std::shared_ptr<Entity> cameraEntity = std::make_shared<Entity>();
    const auto cameraTr = cameraEntity->AddComponent<Transform>();
    cameraTr.lock()->SetPosition(glm::vec3(0, 0, -2));
    cameraTr.lock()->SetEulerRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    const auto camera = cameraEntity->AddComponent<CameraComponent>();
    camera.lock()->SetCamera(Camera::Perspective(window));

    RenderPipeline renderPipeline = {camera, cameraTr, window};
    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(texture_face);

    std::shared_ptr<Entity> e2 = std::make_shared<Entity>();
    auto textRedenrer = e2->AddComponent<UiTextRenderer>();

    textRedenrer.lock()->SetMaterial(materialText);

    textRedenrer.lock()->SetText("+++++++++12345667790asdfgdkmbkmfv");
    auto tr1 = e2->AddComponent<RectTransform>();
    tr1.lock()->SetParent(rectRoot);

    renderPipeline.AddRenderer(textRedenrer);

    std::shared_ptr<Entity> e3 = std::make_shared<Entity>();
    const auto uiImage = e3->AddComponent<UiImageRenderer>();
    auto tr3 = e3->AddComponent<RectTransform>();

    uiImage.lock()->SetMaterial(materialUI);
    uiImage.lock()->SetSprite(sprite);

    tr3.lock()->SetParent(rectRoot);

    tr3.lock()->AddLayoutOption(std::make_unique<CenterXLayoutOption>());
    tr3.lock()->AddLayoutOption(std::make_unique<CenterYLayoutOption>());

    tr3.lock()->AddLayoutOption(std::make_unique<PixelWidthLayoutOption>(200));
    tr3.lock()->AddLayoutOption(std::make_unique<PixelHeightLayoutOption>(200));

    tr3.lock()->AddLayoutOption(std::make_unique<PivotLayoutOption>(glm::vec2(0.5f, 0.5f)));

    renderPipeline.AddRenderer(uiImage);

    std::shared_ptr<Entity> e4 = std::make_shared<Entity>();
    const auto uiImage4 = e4->AddComponent<UiImageRenderer>();
    auto tr4 = e4->AddComponent<RectTransform>();

    uiImage4.lock()->SetMaterial(materialUI);
    uiImage4.lock()->SetSprite(sprite);

    tr4.lock()->SetParent(rectRoot);

    tr4.lock()->AddLayoutOption(std::make_unique<CenterXLayoutOption>());
    tr4.lock()->AddLayoutOption(std::make_unique<CenterYLayoutOption>());

    tr4.lock()->AddLayoutOption(std::make_unique<PixelWidthLayoutOption>(200));
    tr4.lock()->AddLayoutOption(std::make_unique<PixelHeightLayoutOption>(200));

    tr4.lock()->AddLayoutOption(std::make_unique<PivotLayoutOption>(glm::vec2(0.0, 0.0)));

    renderPipeline.AddRenderer(uiImage4);

    Time time;

    time.Start();

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    while (window->IsOpen())
    {
        // const float deltaTime = time.GetResetDelta();
        time.Reset();

        // textRedenrer.lock()->SetText(std::to_string(1.0f / deltaTime));

        glClear(GL_COLOR_BUFFER_BIT);

        renderPipeline.RenderSprites();
        renderPipeline.RenderUI();

        window->SwapBuffers();
        glfwPollEvents();
    }

    window->Destroy();
    return 0;
}
