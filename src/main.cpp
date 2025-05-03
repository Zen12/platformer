#include <iostream>
#include "engine/engine.h"
#include <chrono>

Font font;

int main() {

    // system init
    auto window = std::make_shared<Window>(800, 600, "Platformer");
    window->WinInit();

    using Clock = std::chrono::high_resolution_clock;

    auto lastTime = Clock::now();

    RenderPipeline renderPipeline = {window};
/*
    auto texture_face = AssetLoader::LoadSpriteFromPath("images/awesomeface.png");
    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(texture_face);

    std::shared_ptr<Entity> cameraEntity = std::make_shared<Entity>();  
    auto cameraTr = cameraEntity->AddComponent<Transform>();
    cameraTr.lock()->SetPosition(glm::vec3(0, 0, 0));
    cameraTr.lock()->SetEulerRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    auto camera = cameraEntity->AddComponent<CameraComponent>();
    camera.lock()->SetCamera(Camera::Ortho());


    std::shared_ptr<Entity> e1 = std::make_shared<Entity>();  
    auto renderer = e1->AddComponent<SpriteRenderer>().lock();
    auto tr = e1->AddComponent<Transform>();
    renderer->SetSprite(sprite);
    renderer->SetCamera(camera);

*/

    std::shared_ptr<Entity> e1 = std::make_shared<Entity>();  
    auto textRedenrer = e1->AddComponent<UiTextRenderer>();
    textRedenrer.lock()->SetText("Hello! Here I am");
    auto tr1 = e1->AddComponent<RectTransform>();
    tr1.lock()->SetRect(glm::vec4(0.0, 0.01, 0.0, 0.0));
    renderPipeline.AddRenderer(textRedenrer);

    std::shared_ptr<Entity> e2 = std::make_shared<Entity>();  
    auto tr2 = e2->AddComponent<RectTransform>();
    tr2.lock()->SetRect(glm::vec4(0.0, 0.02, 0, 0));
    auto textRedenrer2 = e2->AddComponent<UiTextRenderer>();
    textRedenrer2.lock()->SetText("LA!");
    renderPipeline.AddRenderer(textRedenrer2);


    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


    while (window->IsOpen()) {

        auto currentTime = Clock::now();
        std::chrono::duration<float> duration = currentTime - lastTime;
        lastTime = currentTime;
        const float deltaTime = duration.count();

        textRedenrer.lock()->SetText(std::to_string(1.0f / deltaTime));

        glClear(GL_COLOR_BUFFER_BIT);

        glClear(GL_COLOR_BUFFER_BIT);

        //e1->Update();
        renderPipeline.RenderUI();

        window->SwapBuffers();
        glfwPollEvents();
    }


    window->Destroy();
    return 0;
}

