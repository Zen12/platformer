#include <iostream>
#include "engine/engine.h"
#include <chrono>

Font font;

int main() {

    // system init
    auto window = std::make_shared<Window>(800, 600, "Platformer");
    window->WinInit();

    std::shared_ptr<Shader> uiShader = std::make_shared<Shader>(AssetLoader::LoadShaderFromPath("shaders/uiImage_vert.glsl", "shaders/uiImage_frag.glsl"));  
    std::shared_ptr<Shader> uiText = std::make_shared<Shader>(AssetLoader::LoadShaderFromPath("shaders/text_vert.glsl", "shaders/text_frag.glsl"));  
    auto texture_face = AssetLoader::LoadSpriteFromPath("images/awesomeface.png");


    std::shared_ptr<Material> materialUI = std::make_shared<Material>(uiShader);
    std::shared_ptr<Material> materialText = std::make_shared<Material>(uiText);

    std::shared_ptr<Font> font = std::make_shared<Font>(AssetLoader::LoadFontFromPath("fonts/Antonio-Bold.ttf"));

    materialText->SetFont(font);

    using Clock = std::chrono::high_resolution_clock;

    auto lastTime = Clock::now();

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
    tr1.lock()->SetRect(glm::vec4(0.0, 0.5, 0.5, 0.5));
    renderPipeline.AddRenderer(textRedenrer);



    std::shared_ptr<Entity> e3 = std::make_shared<Entity>();  
    const auto uiImage = e3->AddComponent<UiImageRenderer>();
    auto tr3 = e3->AddComponent<RectTransform>();
    tr3.lock()->SetRect(glm::vec4(0.5,0.5,0.05,0.05));
    uiImage.lock()->SetMaterial(materialUI);
    uiImage.lock()->SetSprite(sprite);
    renderPipeline.AddRenderer(uiImage);


    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


    while (window->IsOpen()) {

        auto currentTime = Clock::now();
        std::chrono::duration<float> duration = currentTime - lastTime;
        lastTime = currentTime;
        const float deltaTime = duration.count();

        //textRedenrer.lock()->SetText(std::to_string(1.0f / deltaTime));

        glClear(GL_COLOR_BUFFER_BIT);

        renderPipeline.RenderSprites();
        renderPipeline.RenderUI();

        window->SwapBuffers();
        glfwPollEvents();
    }


    window->Destroy();
    return 0;
}

