#pragma once
#include "axion_engine/core/EngineContext.hpp"

class ContextAware
{
public:
    explicit ContextAware(EngineContext &ctx) : ctx_(ctx) {}

protected:
    EngineContext &ctx_;

    //Shorthands
    Window& window() const noexcept { return *ctx_.window; };
    RenderManager& render() const noexcept { return *ctx_.render; };
    InputManager& input() const noexcept { return *ctx_.input; };
    TimeManager& time() const noexcept { return *ctx_.time; };
    SceneManager& scene() const noexcept { return *ctx_.scene; };
    PhysicsManager& physics() const noexcept { return *ctx_.physics; };
    AssetsManager& assets() const noexcept { return *ctx_.assets; }
};