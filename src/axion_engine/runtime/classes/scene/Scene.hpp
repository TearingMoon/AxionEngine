#pragma once

#include <memory>
#include <vector>
#include <algorithm>

#include "axion_engine/structure/ContextAware.hpp"
#include "axion_engine/core/EngineContext.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"
#include "axion_engine/platform/window/Window.hpp"

class Scene : public ContextAware
{
    friend class SceneManager;
    friend class PhysicsManager;

public:
    using ContextAware::ContextAware;

    virtual ~Scene() = default;

    void Tick();
    void Draw();

    GameObject *CreateGameObject();
    void DestroyGameObject(GameObject &object);

    std::vector<GameObject *> GetGameObjects() const
    {
        std::vector<GameObject *> ptrs;
        ptrs.reserve(objects_.size());
        for (const auto &obj : objects_)
        {
            ptrs.push_back(obj.get());
        }
        return ptrs;
    }

    EngineContext &GetContext() { return ctx_; }
    CameraComponent *GetCurrentCamera() const { return currentCamera_; }

protected:
    virtual void OnSceneEnter() {}
    virtual void OnSceneExit() {}

    virtual void OnSceneUpdate() {}
    virtual void OnSceneRender() {}

    void SetCurrentCamera(CameraComponent *camera) { currentCamera_ = camera; }

    SDL_Texture *LoadTexture(const std::string &path) //TODO: Check this, i dont really like it
    {
        if (ctx_.window && ctx_.window->GetRenderer())
        {
            SDL_Renderer *renderer = ctx_.window->GetRenderer();
            SDL_Texture *texture = ctx_.window->LoadTexture(renderer, path);
            return texture;
        }
        return nullptr;
    }

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
        EmitDestroyedEvent(); // TODO: Destroy all objects
    }

    void EmitMountedEvent();
    void EmitEnabledEvent();

    void EmitDestroyedEvent();
    void EmitDisabledEvent();

    void EmitFixedUpdateEvent();

    void ProcessDestroyQueue();

    std::vector<std::unique_ptr<GameObject>> objects_;
    std::vector<GameObject *> destroyQueue_;

    CameraComponent *currentCamera_ = nullptr;
};