#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_set>

#include "axion_engine/structure/ContextAware.hpp"
#include "axion_engine/core/EngineContext.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"
#include "axion_engine/platform/window/Window.hpp"

namespace Axion
{

/**
 * @brief Base class for game scenes.
 * 
 * A Scene contains and manages GameObjects, handling their lifecycle,
 * updates, and rendering. Derive from Scene to create custom game levels
 * or screens.
 * 
 * @par Usage:
 * @code
 * class MyScene : public Axion::Scene {
 * public:
 *     using Scene::Scene;
 *     
 *     void OnSceneEnter() override {
 *         auto player = CreateGameObject();
 *         // Configure player...
 *     }
 * };
 * @endcode
 */
class Scene : public ContextAware
{
    friend class SceneManager;
    friend class PhysicsManager;

public:
    using ContextAware::ContextAware;

    virtual ~Scene() = default;

    /** @brief Updates all GameObjects in the scene. */
    void Tick();
    
    /** @brief Renders all GameObjects in the scene. */
    void Draw();

    /**
     * @brief Creates a new GameObject in this scene.
     * @return Pointer to the newly created GameObject
     */
    GameObject* CreateGameObject();
    
    /**
     * @brief Marks a GameObject for destruction at frame end.
     * @param object The GameObject to destroy
     */
    void DestroyGameObject(GameObject& object);

    /** @brief Returns all active GameObjects in the scene. */
    std::vector<GameObject*> GetGameObjects() const
    {
        std::vector<GameObject*> ptrs;
        ptrs.reserve(objects_.size());
        for (const auto& obj : objects_)
            ptrs.push_back(obj.get());
        return ptrs;
    }

    /** @brief Returns the engine context. */
    EngineContext& GetContext() { return ctx_; }
    
    /** @brief Returns the current active camera, or nullptr if none. */
    CameraComponent* GetCurrentCamera() const { return currentCamera_; }

    /** @brief World gravity vector applied to rigidbodies. */
    glm::vec3 Gravity = glm::vec3(0.0f, 9.81f, 0.0f);

protected:
    /** @brief Override to set up the scene when it becomes active. */
    virtual void OnSceneEnter() {}
    
    /** @brief Override to clean up when the scene becomes inactive. */
    virtual void OnSceneExit() {}

    /** @brief Override for per-frame scene logic. */
    virtual void OnSceneUpdate() {}
    
    /** @brief Override for custom rendering. */
    virtual void OnSceneRender() {}

    /**
     * @brief Sets the active camera for rendering.
     * @param camera The camera component to use
     */
    void SetCurrentCamera(CameraComponent* camera) { currentCamera_ = camera; }

private:
    void Enter()
    {
        OnSceneEnter();
        EmitMountedEvent();
        EmitEnabledEvent();
    }

    void Exit()
    {
        OnSceneExit();
        EmitDisabledEvent();
        EmitDestroyedEvent();
    }

    void EmitMountedEvent();
    void EmitEnabledEvent();
    void EmitDestroyedEvent();
    void EmitDisabledEvent();
    void EmitFixedUpdateEvent();

    void ProcessDestroyQueue();
    void ProcessSpawnQueue();
    
    void ClearAllObjects()
    {
        // Destroy objects in spawn queue that never made it to the scene
        for (auto& obj : spawnQueue_)
        {
            if (obj)
                obj->OnDestroy();
        }
        spawnQueue_.clear();
        destroyQueue_.clear();
        
        for (auto& obj : objects_)
        {
            if (obj)
                obj->OnDestroy();
        }
        objects_.clear();
        currentCamera_ = nullptr;
    }

    std::vector<std::unique_ptr<GameObject>> objects_;
    std::vector<std::unique_ptr<GameObject>> spawnQueue_;
    std::vector<GameObject*> destroyQueue_;

    CameraComponent* currentCamera_ = nullptr;
};

} // namespace Axion