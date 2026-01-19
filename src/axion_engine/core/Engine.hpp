#pragma once
#include <memory>

#include "EngineConfig.hpp"
#include "EngineContext.hpp"

#include "axion_engine/platform/sdl_manager/SDLManager.hpp"
#include "axion_engine/platform/window/Window.hpp"

#include "axion_engine/managers/logger/Logger.hpp"
#include "axion_engine/managers/analysis/Analyzer.hpp"

#include "axion_engine/managers/time/TimeManager.hpp"
#include "axion_engine/managers/input/InputManager.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"
#include "axion_engine/managers/physics/PhysicsManager.hpp"
#include "axion_engine/managers/render/RenderManager.hpp"
#include "axion_engine/managers/assets/AssetsManager.hpp"

#include "axion_engine/core/events/EventBus.hpp"

namespace Axion
{

/**
 * @brief Represents the current operational state of the engine.
 */
enum class EngineState
{
    Running,    ///< Engine is actively processing frames
    Stopped     ///< Engine has halted or not yet started
};

/**
 * @brief Core engine class that orchestrates all game systems.
 * 
 * The Engine class is the main entry point for the Axion game engine.
 * It initializes all subsystems, manages the main game loop, and coordinates
 * communication between different managers (rendering, physics, input, etc.).
 * 
 * @par Usage Example:
 * @code
 * Axion::EngineConfig config;
 * config.updateDelay = 16;
 * 
 * Axion::WindowConfig windowConfig;
 * windowConfig.title = "My Game";
 * 
 * Axion::Engine engine(config, windowConfig);
 * engine.GetSceneManager()->RegisterScene<MyScene>("Main");
 * engine.GetSceneManager()->RequestChange("Main");
 * engine.Run();
 * @endcode
 * 
 * @note Only one Engine instance should exist at a time.
 */
class Engine
{
public:
    /**
     * @brief Constructs the engine with the specified configuration.
     * @param config Engine configuration settings
     * @param windowConfig Window creation parameters
     */
    Engine(EngineConfig config = {}, WindowConfig windowConfig = {});
    
    /**
     * @brief Destructor. Cleans up all engine resources.
     */
    ~Engine();

    /**
     * @brief Starts the main game loop.
     * 
     * This method blocks until the engine receives a quit signal.
     * The loop processes input, updates game logic, runs physics,
     * and renders each frame.
     */
    void Run();

    /** @brief Returns a pointer to the TimeManager subsystem. */
    TimeManager* GetTimeManager() const { return time_.get(); }
    
    /** @brief Returns a pointer to the InputManager subsystem. */
    InputManager* GetInputManager() const { return input_.get(); }
    
    /** @brief Returns a pointer to the SceneManager subsystem. */
    SceneManager* GetSceneManager() const { return scene_.get(); }

private:
    EngineConfig config_;
    EngineContext context_;

    SDLManager sdlManager_;
    SDL_Event event_ = {};
    Window window_;

    EngineState state_ = EngineState::Stopped;

    std::unique_ptr<Logger> logger_;
    std::unique_ptr<Analyzer> analyzer_;
    std::unique_ptr<EventBus> events_;

    std::unique_ptr<TimeManager> time_;
    std::unique_ptr<InputManager> input_;
    std::unique_ptr<SceneManager> scene_;
    std::unique_ptr<PhysicsManager> physics_;
    std::unique_ptr<RenderManager> render_;
    std::unique_ptr<AssetsManager> assets_;

    void EventLoop();
    void AppLoop();
};

} // namespace Axion