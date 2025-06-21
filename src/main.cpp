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

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    while (window->IsOpen())
    {
        time.Reset();

        glClear(GL_COLOR_BUFFER_BIT);

        renderPipeline->RenderSprites();
        renderPipeline->RenderUI();

        window->SwapBuffers();
        glfwPollEvents();
    }

    sceneManager.UnLoadAll();
    window->Destroy();
}
