#include "SceneManager.hpp"

SceneManager::SceneManager(EngineContext &ctx) : ContextAware(ctx)
{
    printf("- SceneManager initialized. \n");
}

SceneManager::~SceneManager()
{
    printf("- SceneManager destroyed. \n");
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
        printf("Scene '%s' is not registered. Cannot change scene.\n", sceneName.c_str());
        return;
    }

    printf("Changing to scene: %s\n", sceneName.c_str());
    if (currentScene_)
    {
        currentScene_->Exit();
    }

    currentScene_ = std::move(scenes_[sceneName]);

    currentScene_->Enter();
}
