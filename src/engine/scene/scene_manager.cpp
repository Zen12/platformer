#include "scene_manager.hpp"
#include "scene_asset_yaml.hpp"

#include <GL/glew.h>
#include <fstream>
#include "../renderer/ui/opengl_render_interface.hpp"

#define DEBUG_ENGINE_SCENE_MANAGER_PROFILE 0


void SceneManager::LoadScene(const SceneAsset &sceneAsset) {
#ifndef NDEBUG
#if DEBUG_ENGINE_SCENE_MANAGER_PROFILE
    PROFILE_SCOPE("Loading of scene " + sceneAsset.Name);
#endif
#endif

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    if (const auto assetManager = _assetManager.lock()) {
        _scene = std::make_shared<Scene> (_window,_assetManager, _inputSystem);

        LoadEntities(sceneAsset.Entities);

        // Initialize RmlUi with OpenGL render interface (RmlUi 6.0 API)
        static bool rmlui_initialized = false;

        if (rmlui_initialized == false) {
            static OpenGLRenderInterface render_interface;

            // Initialize the render interface with window dimensions and asset path
            render_interface.Initialize(_window, GetMaterial("48389756-b5e5-4e57-a44c-e85a030304c8"));

            Rml::SetRenderInterface(&render_interface);
            Rml::Initialise();

            const auto fontPath = assetManager->GetPathFromGuid("682a2b0c-eeac-48f0-ab67-d2312a971cec");

            Rml::LoadFontFace(fontPath);
            rmlui_initialized = true;
        }
    }

    // Create context
    if (const auto window = _window.lock()) {
        _rmlContext = Rml::CreateContext("main", Rml::Vector2i(window->GetWidth(), window->GetHeight()));

        if (_rmlContext) {
            // Create a simple Hello World document
            Rml::ElementDocument* document = _rmlContext->CreateDocument();
            if (document) {
                document->SetInnerRML(R"(
                    <div style="position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%);
                         font-family: arial; font-size: 48px; color: #FFFFFF; text-align: center;">
                        Hello World from RmlUi!
                    </div>
                )");
                document->Show();
            }
        }
    }

}

void SceneManager::LoadEntities(const std::vector<EntitySerialization> &serialization) {

    _escSystem = std::make_unique<EscSystem>(_scene);

    _escSystem->LoadEntities(serialization);

    _escSystem->InitSystems();
}


void SceneManager::Update() {
#ifndef NDEBUG
#if DEBUG_ENGINE_SCENE_MANAGER_PROFILE
    PROFILE_SCOPE("  SceneManager::Update");
#endif
#endif
    glClear(GL_COLOR_BUFFER_BIT);

    _escSystem->Update(); // ATM single threaded

    // Render RmlUi with proper 2D OpenGL state
    if (_rmlContext) {
        _rmlContext->Update();

        // Set up 2D rendering state
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        _rmlContext->Render();

        // Restore state
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    // here we should do rendering
}

std::shared_ptr<Shader> SceneManager::GetShader(const std::string &vertexGuid, const std::string &fragmentGuid) const {
    return _scene->GetShader(vertexGuid, fragmentGuid);
}

std::shared_ptr<Material> SceneManager::GetMaterial(const std::string &guid) const {
    return _scene->GetMaterial(guid);
}

bool SceneManager::IsRequestToLoadScene() const {
    return !_scene->GetLoadSceneRequestGuid().empty();
}

void SceneManager::LoadRequestedScene() {
    if (!_scene->GetLoadSceneRequestGuid().empty()) {
        if (const auto assetManager = _assetManager.lock()) {
            const auto scene = assetManager->LoadAssetByGuid<SceneAsset>(_scene->GetLoadSceneRequestGuid());
            LoadScene(scene);
        }
    }
}

std::shared_ptr<Texture> SceneManager::GetSprite(const std::string &guid) const {
    return _scene->GetTexture(guid);
}

std::shared_ptr<Font> SceneManager::GetFont(const std::string &guid) const {
   return _scene->GetFont(guid);
}

SceneManager::~SceneManager() {
    // Clean up RmlUi context before shutdown
    if (_rmlContext) {
        // Remove the context properly - this will release all associated resources
        Rml::RemoveContext(_rmlContext->GetName());
        _rmlContext = nullptr;
    }
}
