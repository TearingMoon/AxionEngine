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

namespace Axion
{

/**
 * @brief Represents the operational state of the SceneManager.
 */
enum class SceneManagerState
{
    NoCurrentScene,     ///< No scene is currently active
    NoScenes,           ///< No scenes have been registered
    Running             ///< A scene is active and running
};

/**
 * @brief Manages scene lifecycle and transitions.
 * 
 * SceneManager handles scene registration, loading, and transitions.
 * It supports deferred scene changes to ensure safe transitions during
 * the game loop.
 * 
 * @par Usage:
 * @code
 * sceneManager->RegisterScene<MyScene>("Main");
 * sceneManager->RequestChange("Main");
 * @endcode
 */
class SceneManager : public ContextAware
{
public:
    SceneManager(EngineContext& ctx);
    ~SceneManager();

    /** @brief Updates the current scene. Called each frame. */
    void Update();

    /**
     * @brief Requests a scene change (processed at frame end).
     * @param sceneName Name of the registered scene to switch to
     */
    void RequestChange(std::string sceneName);

    /** @brief Processes pending scene change requests and object queues. */
    void ProcessRequests();

    /**
     * @brief Registers a scene type with a given name.
     * @tparam TScene Scene class type (must derive from Scene)
     * @tparam Args Constructor argument types
     * @param sceneName Unique identifier for the scene
     * @param args Arguments forwarded to the scene constructor
     */
    template <typename TScene, typename... Args>
    void RegisterScene(const std::string& sceneName, Args&&... args);

    /**
     * @brief Checks if a scene with the given name is registered.
     * @param sceneName Name to check
     * @return True if the scene exists
     */
    bool IsSceneRegistered(std::string sceneName) const;

    /** @brief Returns the currently active scene, or nullptr if none. */
    Scene* GetCurrentScene() const { return currentScene_; }

private:
    void ChangeScene(std::string sceneName);

    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes_;
    Scene* currentScene_ = nullptr;
    std::string currentSceneName_;

    std::queue<std::string> sceneChangeRequests_;
};

template <typename TScene, typename... Args>
inline void SceneManager::RegisterScene(const std::string& sceneName, Args&&... args)
{
    static_assert(std::is_base_of_v<Scene, TScene>, "TScene must derive from Scene");

    scenes_[sceneName] = std::make_unique<TScene>(
        ctx_,
        std::forward<Args>(args)...);
}

} // namespace Axion
