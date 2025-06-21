#include <iostream>
#include "engine/engine.hpp"

int main()
{
    std::string projectRoot = ASSETS_PATH;
    const ProjectAsset projectAsset = AssetLoader::LoadProjectAssetFromPath(projectRoot + "project.yaml");

    auto assetManager = std::make_shared<AssetManager>(projectRoot);
    assetManager->Init();

    auto window = std::make_shared<Window>(800, 600, projectAsset.Name);
    window->WinInit();

    const auto scene = assetManager->LoadSceneByGuid(projectAsset.Scenes[0]);

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
        window->PullEvent();
    }

    sceneManager.UnLoadAll();
    window->Destroy();
}
