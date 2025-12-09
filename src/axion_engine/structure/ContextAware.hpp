#pragma once

#include "axion_engine/core/EngineContext.hpp"
#include "axion_engine/managers/logger/Logger.hpp"

#define INFO(fmt, ...) ctx_.logger->Info(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) ctx_.logger->Warn(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) ctx_.logger->Error(fmt, ##__VA_ARGS__)

class ContextAware
{
public:
    explicit ContextAware(EngineContext &ctx) : ctx_(ctx) {}

protected:
    EngineContext &ctx_;

    // Shorthands
    Window &window() const noexcept { return *ctx_.window; };
    RenderManager &render() const noexcept { return *ctx_.render; };
    InputManager &input() const noexcept { return *ctx_.input; };
    TimeManager &time() const noexcept { return *ctx_.time; };
    SceneManager &scene() const noexcept { return *ctx_.scene; };
    PhysicsManager &physics() const noexcept { return *ctx_.physics; };
    AssetsManager &assets() const noexcept { return *ctx_.assets; }
    Logger &logger() const noexcept { return *ctx_.logger; };
};