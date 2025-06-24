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

    Time time;

    time.Start();

    renderPipeline->Init();

    while (window->IsOpen())
    {
        time.Reset();

        renderPipeline->ClearFrame();
        renderPipeline->RenderSprites();
        renderPipeline->RenderUI();

        window->SwapBuffers();

        window->ClearInputState();
        window->PullEvent();
        input->Update();

#ifndef NDEBUG
        if (input->IsKeyUp(InputKey::Escape)) {
            window->Destroy();
        }
#endif
    }

    sceneManager.UnLoadAll();
    window->Destroy();
}
