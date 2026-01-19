#pragma once
#include <cstdio>
#include <memory>
#include <string>
#include <unordered_map>
#include <queue>

#include "axion_engine/structure/ContextAware.hpp"
#include "axion_engine/core/EngineContext.hpp"
#include "axion_engine/managers/time/TimeManager.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"

enum SCENE_MANAGER_STATE // TODO: Implement state handling
{
    SCENE_MANAGER_NO_CURRENT_SCENE,
    SCENE_MANAGER_NO_SCENES,
    SCENE_MANAGER_RUNNING
};

class SceneManager : public ContextAware
{
public:
    SceneManager(EngineContext &ctx);
    ~SceneManager();

    void Update();

    void RequestChange(std::string sceneName);

    void ProcessRequests();

    template <typename TScene, typename... Args>
    void RegisterScene(const std::string &sceneName, Args &&...args);

    bool IsSceneRegistered(std::string sceneName) const;

    Scene *GetCurrentScene() const { return currentScene_; }

private:
    void ChangeScene(std::string sceneName);

    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes_;
    Scene *currentScene_ = nullptr;
    std::string currentSceneName_;

    std::queue<std::string> sceneChangeRequests_;
};

template <typename TScene, typename... Args>
inline void SceneManager::RegisterScene(const std::string &sceneName, Args &&...args)
{
    static_assert(std::is_base_of_v<Scene, TScene>, "TScene must derive from Scene");

    scenes_[sceneName] = std::make_unique<TScene>(
        ctx_,
        std::forward<Args>(args)...);
}
