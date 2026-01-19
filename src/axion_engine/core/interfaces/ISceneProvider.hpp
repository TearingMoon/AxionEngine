#pragma once

namespace Axion
{

class Scene;

/**
 * @brief Interface for scene management operations.
 * 
 * Provides limited access to scene transitions without
 * exposing the full SceneManager implementation.
 */
class ISceneProvider
{
public:
    virtual ~ISceneProvider() = default;
    
    /**
     * @brief Requests a scene change (processed at frame end).
     * @param sceneName Name of the registered scene to switch to
     */
    virtual void RequestChange(const std::string& sceneName) = 0;
    
    /**
     * @brief Returns the currently active scene.
     * @return Pointer to current scene or nullptr
     */
    virtual Scene* GetCurrentScene() const = 0;
};

} // namespace Axion
