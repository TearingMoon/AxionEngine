#include "SceneManager.hpp"

namespace Axion
{

SceneManager::SceneManager(EngineContext& ctx) : ContextAware(ctx)
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
    }
}

void SceneManager::RequestChange(std::string sceneName)
{
    sceneChangeRequests_.push(sceneName);
}

void SceneManager::ProcessRequests()
{
    if (currentScene_)
    {
        currentScene_->ProcessSpawnQueue();
        currentScene_->ProcessDestroyQueue();
    }

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

    if (currentSceneName_ == sceneName && currentScene_)
    {
        currentScene_->ClearAllObjects();
    }

    currentScene_ = scenes_[sceneName].get();
    currentSceneName_ = sceneName;

    currentScene_->Enter();
}

} // namespace Axion
