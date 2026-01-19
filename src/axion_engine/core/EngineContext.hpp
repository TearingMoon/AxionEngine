#pragma once

#include "axion_engine/core/interfaces/ITimeProvider.hpp"
#include "axion_engine/core/interfaces/IInputProvider.hpp"
#include "axion_engine/core/interfaces/IAssetProvider.hpp"
#include "axion_engine/core/interfaces/ISceneProvider.hpp"

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
class EventBus;

/**
 * @brief Central context providing access to all engine subsystems.
 * 
 * EngineContext serves as a service locator pattern implementation,
 * providing components and managers with access to engine services
 * without tight coupling to the Engine class itself.
 * 
 * @par Interface-based access (recommended for components):
 * Use the interface pointers (timeProvider, inputProvider, etc.) when you
 * only need read-only or limited functionality. This follows the
 * Interface Segregation Principle (ISP).
 * 
 * @par Manager access (for engine internals):
 * Use the manager pointers (time, input, etc.) when you need full
 * manager functionality. This is typically only needed by engine subsystems.
 * 
 * @note All pointers are non-owning. Lifetime is managed by the Engine class.
 */
struct EngineContext
{
    // === Interface-based access (ISP compliant) ===
    ITimeProvider* timeProvider = nullptr;      ///< Time queries (read-only)
    IInputProvider* inputProvider = nullptr;    ///< Input queries (read-only)
    IAssetProvider* assetProvider = nullptr;    ///< Asset loading interface
    ISceneProvider* sceneProvider = nullptr;    ///< Scene management interface
    
    // === Full manager access (for engine internals) ===
    Logger* logger = nullptr;           ///< Logging subsystem
    Analyzer* analyzer = nullptr;       ///< Performance analysis subsystem
    TimeManager* time = nullptr;        ///< Time and delta time management
    InputManager* input = nullptr;      ///< Input handling subsystem
    SceneManager* scene = nullptr;      ///< Scene lifecycle management
    PhysicsManager* physics = nullptr;  ///< Physics simulation subsystem
    RenderManager* render = nullptr;    ///< Rendering subsystem
    AssetsManager* assets = nullptr;    ///< Asset loading and caching
    Window* window = nullptr;           ///< Window and renderer access
    EventBus* events = nullptr;         ///< Event publish-subscribe system
};

} // namespace Axion