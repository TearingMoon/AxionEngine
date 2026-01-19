#pragma once
#include <cstdio>

#include <SDL.h>
#include <glm/glm.hpp>

#include "axion_engine/structure/ContextAware.hpp"
#include "axion_engine/core/EngineContext.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"
#include "axion_engine/managers/input/InputManager.hpp"
#include "axion_engine/platform/window/Window.hpp"

#include "axion_engine/runtime/components/renderer/RenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/SpriteRenderComponent.hpp"

namespace Axion
{

/**
 * @brief Manages rendering of all visible objects in the scene.
 * 
 * RenderManager collects renderable components, sorts them by layer
 * and order, and renders them to the screen using SDL2.
 * 
 * @par Render Layers:
 * - Background (0)
 * - Default (100) 
 * - Foreground (200)
 * - Debug (250) - Only visible in debug mode (F3)
 * - UI (300)
 */
class RenderManager : public ContextAware
{
public:
    RenderManager(EngineContext& ctx);
    ~RenderManager();

    /** @brief Renders all visible objects in the current scene. */
    void Update();

private:
    std::vector<IRenderable*> renderables;
    void CollectRenderables(Scene& scene, std::vector<IRenderable*>& out);

    SDL_Renderer* renderer_;
    bool debugMode_ = false;
};

} // namespace Axion