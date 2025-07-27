#include <iostream>

#include "engine/engine.hpp"



int main()
{
    std::string projectRoot = ASSETS_PATH;
    const ProjectAsset projectAsset = AssetLoader::LoadFromPath<ProjectAsset>(projectRoot + "project.yaml");

    auto assetManager = std::make_shared<AssetManager>(projectRoot);
    assetManager->Init();

    auto window = std::make_shared<Window>(projectAsset.Resolution[0], projectAsset.Resolution[1], projectAsset.Name);
    window->WinInit();

    const auto input = std::make_shared<InputSystem>(window);

    const auto scene = assetManager->LoadAssetByGuid<SceneAsset>(projectAsset.Scenes[0]);

    const auto renderPipeline = std::make_shared<RenderPipeline>(window);

    auto sceneManager = SceneManager(renderPipeline, window,assetManager);

    sceneManager.LoadScene(scene);

    auto physicsWorld = sceneManager.GetPhysicsWorld();

    Time time;

    time.Start();

    renderPipeline->Init();

    const auto fpsText = sceneManager.GetEntityById("text-fps").lock()->GetComponent<UiTextRenderer>().lock();


    while (window->IsOpen())
    {
        const float deltaTime = time.GetResetDelta();
        time.Reset();

        const float fps = 1.0f / deltaTime;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(0) << fps;
        fpsText->SetText(ss.str());

        const auto character = sceneManager.GetEntityById("main-character").lock()->GetComponent<Transform>().lock();
        const auto world =  physicsWorld->GetWorld().lock();


        auto position = character->GetPosition();

        input->Update();

        const auto speed = 2.0f;

        if (input->IsKeyPressing(InputKey::W) || input->IsKeyPress(InputKey::W)) {
            position.y += speed * deltaTime;
        }

        if (input->IsKeyPressing(InputKey::S)|| input->IsKeyPress(InputKey::S)) {
            position.y -= speed* deltaTime;
        }
        if (input->IsKeyPressing(InputKey::A) || input->IsKeyPress(InputKey::A)) {
            position.x -= speed * deltaTime;
        }
        if (input->IsKeyPressing(InputKey::D) || input->IsKeyPress(InputKey::D)) {
            position.x += speed * deltaTime;
        }

        character->SetPosition(position);

        sceneManager.Update();

        renderPipeline->ClearFrame();
        renderPipeline->RenderLights();
        renderPipeline->RenderMeshes();
        renderPipeline->RenderSprites();

#ifndef NDEBUG
        renderPipeline->RenderLines();
#endif
        renderPipeline->RenderUI();

        window->SwapBuffers();




#ifndef NDEBUG
        if (input->IsKeyUp(InputKey::Escape)) {
            window->Destroy();
        }
#endif
    }

    sceneManager.UnLoadAll();
    window->Destroy();
}
