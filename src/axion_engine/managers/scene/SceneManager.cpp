#include "SceneManager.hpp"

SceneManager::SceneManager(EngineContext &ctx) : ContextAware(ctx)
{
    INFO("SceneManager initialized");
}

SceneManager::~SceneManager()
{
    INFO("SceneManager destroyed");
}

void SceneManager::Update()
{
    if (currentScene_)
    {
        float dt = ctx_.time->GetDeltaTime();
        currentScene_->Tick();
        // currentScene_->Draw(ctx_);
    }
}

void SceneManager::RequestChange(std::string sceneName)
{
    sceneChangeRequests_.push(sceneName);
}

void SceneManager::ProcessRequests()
{
    // Process current scene queues
    if (currentScene_)
    {
        // First, add newly spawned objects to main list
        currentScene_->ProcessSpawnQueue();
        // Then, destroy objects that were marked for destruction
        currentScene_->ProcessDestroyQueue();
    }

    // Process all scene change requests
    while (!sceneChangeRequests_.empty())
    {
        std::string next = sceneChangeRequests_.front();
        sceneChangeRequests_.pop();
        ChangeScene(next);
    }
}

bool SceneManager::IsSceneRegistered(std::string sceneName) const
{
    return scenes_.find(sceneName) != scenes_.end();
}

void SceneManager::ChangeScene(std::string sceneName)
{
    if (!IsSceneRegistered(sceneName))
    {
        WARN("Scene '{}' is not registered. Cannot change scene.", sceneName.c_str());
        return;
    }

    INFO("Changing to scene: {}", sceneName.c_str());
    
    // Exit current scene if exists
    if (currentScene_)
    {
        currentScene_->Exit();
    }

    // If reloading the same scene, we need to reset it
    if (currentSceneName_ == sceneName && currentScene_)
    {
        // Clear all objects and reset the scene
        currentScene_->ClearAllObjects();
    }

    // Set current scene pointer (scene stays in the map)
    currentScene_ = scenes_[sceneName].get();
    currentSceneName_ = sceneName;

    currentScene_->Enter();
}
