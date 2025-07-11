#include <iostream>
#include <thread>

#include "engine/engine.hpp"

class RayCastClosestCallback : public b2RayCastCallback {
public:
    b2Vec2 point;
    b2Vec2 normal;
    float fraction = 1.0f;
    bool hit = false;

    float ReportFixture(b2Fixture* fixture, const b2Vec2& p,
                        const b2Vec2& n, float f) override {
        hit = true;
        point = p;
        normal = n;
        fraction = f;
        return f; // return the fraction to clip the ray and get closest hit
    }
};

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

        physicsWorld->Simulate(fixedTimeStep);
        physicsWorld->UpdateColliders();

        const auto line = sceneManager.GetEntityById("line-render").lock()->GetComponent<LineRenderer>().lock();
        const auto character = sceneManager.GetEntityById("main-character").lock()->GetComponent<Transform>().lock();
        const auto world =  physicsWorld->GetWorld().lock();


        auto position = character->GetPosition();

        window->ClearInputState();
        window->PullEvent();
        input->Update();

        const auto speed = 0.01f;

        if (input->IsKeyPressing(InputKey::W)) {
            position.y += speed * deltaTime;
        }

        if (input->IsKeyPressing(InputKey::S)) {
            position.y -= speed* deltaTime;
        }
        if (input->IsKeyPressing(InputKey::A)) {
            position.x -= speed * deltaTime;
        }
        if (input->IsKeyPressing(InputKey::D)) {
            position.x += speed * deltaTime;
        }

        character->SetPosition(position);

        b2Vec2 pointA(position.x, position.y);    // Start of ray
        b2Vec2 pointB(position.x + 3, position.y + 3);   // End of ray

        RayCastClosestCallback callback;
        world->RayCast(&callback, pointA, pointB);

        if (callback.hit) {
            line->SetPosition(glm::vec3(position.x,position.y,0), glm::vec3(callback.point.x, callback.point.y, 0.0f));
        }

        sceneManager.Update();

        renderPipeline->ClearFrame();
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
