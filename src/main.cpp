#include <iostream>
#include "engine/engine.h"


int main() {

    // system init
    auto window = Window(800, 600, "Platformer");
    
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


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


    while (window.IsOpen()) {
        glClear(GL_COLOR_BUFFER_BIT);
        tr.lock()->SetPosition(glm::vec3(0,0,-10));
        e1->Update();
        window.SwapBuffers();
        glfwPollEvents();
    }


    window.Destroy();
    return 0;
}
