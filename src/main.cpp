#include <iostream>
#include <thread>

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

    auto testEntity = sceneManager.GetEntityById("sprite-test");

    Time time;

    time.Start();

    renderPipeline->Init();

    float accumulator = 0.0f;

    constexpr float targetFPS = 60.0f;
    constexpr float targetFrameTime = 1.0f / targetFPS;  // ~0.01666s

    while (window->IsOpen())
    {
        const float deltaTime = time.GetTimeAlive();
        if (deltaTime <= targetFrameTime) {
            std::this_thread::sleep_for(std::chrono::duration<float>(0.01f));
            continue;
        }

        time.Reset();

        constexpr float fixedTimeStep = 1.0f / 60.0f;

        accumulator += deltaTime;
        while (accumulator >= fixedTimeStep) {
            physicsWorld->Simulate(fixedTimeStep);
            physicsWorld->UpdateColliders();
            accumulator -= fixedTimeStep;
        }

        sceneManager.Update();

        renderPipeline->ClearFrame();
        renderPipeline->RenderSprites();
        renderPipeline->RenderUI();

        window->SwapBuffers();

        window->ClearInputState();
        window->PullEvent();
        input->Update();

        if (input->IsKeyPressing(InputKey::D)) {
            testEntity.lock()->GetComponent<Rigidbody2dComponent>().lock()
            ->AddForce(glm::vec2(300, 0));
        }

        if (input->IsKeyPressing(InputKey::A)) {
            testEntity.lock()->GetComponent<Rigidbody2dComponent>().lock()
            ->AddForce(glm::vec2(-300, 0));
        }


        if (input->IsKeyPressing(InputKey::W)) {
            testEntity.lock()->GetComponent<Rigidbody2dComponent>().lock()
            ->AddForce(glm::vec2(0, 1000000));
        }

#ifndef NDEBUG
        if (input->IsKeyUp(InputKey::Escape)) {
            window->Destroy();
        }
#endif
    }

    sceneManager.UnLoadAll();
    window->Destroy();
}
