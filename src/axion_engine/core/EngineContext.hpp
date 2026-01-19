#pragma once

namespace Axion
{

class Logger;
class Analyzer;
class TimeManager;
class InputManager;
class SceneManager;
class PhysicsManager;
class RenderManager;
class AssetsManager;
class Window;

/**
 * @brief Central context providing access to all engine subsystems.
 * 
 * EngineContext serves as a service locator pattern implementation,
 * providing components and managers with access to engine services
 * without tight coupling to the Engine class itself.
 * 
 * @note All pointers are non-owning. Lifetime is managed by the Engine class.
 */
struct EngineContext
{
    Logger* logger = nullptr;           ///< Logging subsystem
    Analyzer* analyzer = nullptr;       ///< Performance analysis subsystem
    TimeManager* time = nullptr;        ///< Time and delta time management
    InputManager* input = nullptr;      ///< Input handling subsystem
    SceneManager* scene = nullptr;      ///< Scene lifecycle management
    PhysicsManager* physics = nullptr;  ///< Physics simulation subsystem
    RenderManager* render = nullptr;    ///< Rendering subsystem
    AssetsManager* assets = nullptr;    ///< Asset loading and caching
    Window* window = nullptr;           ///< Window and renderer access
};

} // namespace Axion