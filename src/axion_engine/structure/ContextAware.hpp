#pragma once

#include "axion_engine/core/EngineContext.hpp"
#include "axion_engine/managers/logger/Logger.hpp"

namespace Axion
{

#define AXION_INFO(fmt, ...) ctx_.logger->Info(fmt, ##__VA_ARGS__)
#define AXION_WARN(fmt, ...) ctx_.logger->Warn(fmt, ##__VA_ARGS__)
#define AXION_ERROR(fmt, ...) ctx_.logger->Error(fmt, ##__VA_ARGS__)

/**
 * @brief Base class for objects that require access to the engine context.
 * 
 * ContextAware provides a common interface for classes that need to interact
 * with engine subsystems. It stores a reference to the EngineContext and
 * provides convenient accessor methods for commonly used managers.
 * 
 * Classes deriving from ContextAware gain access to logging macros
 * (AXION_INFO, AXION_WARN, AXION_ERROR)
 * and shorthand accessors for all engine managers.
 */
class ContextAware
{
public:
    /**
     * @brief Constructs a ContextAware object with the given engine context.
     * @param ctx Reference to the engine context
     */
    explicit ContextAware(EngineContext& ctx) : ctx_(ctx) {}

protected:
    EngineContext& ctx_;

    Window& window() const noexcept { return *ctx_.window; }
    RenderManager& render() const noexcept { return *ctx_.render; }
    InputManager& input() const noexcept { return *ctx_.input; }
    TimeManager& time() const noexcept { return *ctx_.time; }
    SceneManager& scene() const noexcept { return *ctx_.scene; }
    PhysicsManager& physics() const noexcept { return *ctx_.physics; }
    AssetsManager& assets() const noexcept { return *ctx_.assets; }
    Logger& logger() const noexcept { return *ctx_.logger; }
};

} // namespace Axion