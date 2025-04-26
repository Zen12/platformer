#include <iostream>
#include "engine/engine.h"

Font font;

int main() {

    // system init
    auto window = Window(800, 600, "Platformer");
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


    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


    while (window.IsOpen()) {
        glClear(GL_COLOR_BUFFER_BIT);
        //tr.lock()->SetPosition(glm::vec3(0,0,-10));
        //e1->Update();

        glClear(GL_COLOR_BUFFER_BIT);

        e1->Update();

       // RenderText(shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        //RenderText(shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

        window.SwapBuffers();
        glfwPollEvents();
    }


    window.Destroy();
    return 0;
}

