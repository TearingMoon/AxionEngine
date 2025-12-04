#pragma once
#include <memory>

#include "EngineConfig.hpp"
#include "EngineContext.hpp"

#include "axion_engine/platform/sdl_manager/SDLManager.hpp"
#include "axion_engine/platform/window/Window.hpp"

#include "axion_engine/managers/time/TimeManager.hpp"
#include "axion_engine/managers/input/InputManager.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"
#include "axion_engine/managers/physics/PhysicsManager.hpp"
#include "axion_engine/managers/render/RenderManager.hpp"
#include "axion_engine/managers/assets/AssetsManager.hpp"

enum ENGINE_STATE
{
    ENGINE_STATE_RUNNING,
    ENGINE_STATE_STOPPED
};

// Single Engine functionality for now
class Engine
{
public:
    Engine(EngineConfig config = {}, WindowConfig WindowConfig = {});
    ~Engine();

    void Run();

    TimeManager *GetTimeManager() const { return time_.get(); }
    InputManager *GetInputManager() const { return input_.get(); }
    SceneManager *GetSceneManager() const { return scene_.get(); }

private:
    EngineConfig config_;
    EngineContext context_;

    SDLManager sdlManager_;
    SDL_Event event_ = {};
    Window window_;

    ENGINE_STATE state_ = ENGINE_STATE_STOPPED;

    // Managers
    std::unique_ptr<TimeManager> time_;
    std::unique_ptr<InputManager> input_;
    std::unique_ptr<SceneManager> scene_;
    std::unique_ptr<PhysicsManager> physics_;
    std::unique_ptr<RenderManager> render_;
    std::unique_ptr<AssetsManager> assets_;

    void EventLoop();
    void AppLoop();
};