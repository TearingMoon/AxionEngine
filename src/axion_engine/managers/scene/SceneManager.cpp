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
    // Process current scene Destroy queue
    if (currentScene_)
    {
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
    if (currentScene_)
    {
        currentScene_->Exit();
    }

    currentScene_ = std::move(scenes_[sceneName]);

    currentScene_->Enter();
}
