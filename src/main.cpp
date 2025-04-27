#include <iostream>
#include "engine/engine.h"

Font font;

int main() {

    // system init
    auto window = std::make_shared<Window>(800, 600, "Platformer");
    window->WinInit();

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
    auto tr1 = e1->AddComponent<Transform>();
    tr1.lock()->SetPosition(glm::vec3(0.0, 0.004, 0));
    renderPipeline.AddRenderer(textRedenrer);

    std::shared_ptr<Entity> e2 = std::make_shared<Entity>();  
    auto tr2 = e2->AddComponent<Transform>();
    tr2.lock()->SetPosition(glm::vec3(-0.001, 0.0, 0));
    auto textRedenrer2 = e2->AddComponent<UiTextRenderer>();
    textRedenrer2.lock()->SetText("LA!");
    renderPipeline.AddRenderer(textRedenrer2);


    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


    while (window->IsOpen()) {
        glClear(GL_COLOR_BUFFER_BIT);
        //tr.lock()->SetPosition(glm::vec3(0,0,-10));
        //e1->Update();

        glClear(GL_COLOR_BUFFER_BIT);

        //e1->Update();
        renderPipeline.RenderUI();

        window->SwapBuffers();
        glfwPollEvents();
    }


    window->Destroy();
    return 0;
}

