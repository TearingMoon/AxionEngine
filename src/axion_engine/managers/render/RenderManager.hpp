#pragma once
#include <cstdio>

#include <SDL.h>
#include <glm/glm.hpp>

#include "axion_engine/structure/ContextAware.hpp"
#include "axion_engine/core/EngineContext.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"
#include "axion_engine/platform/window/Window.hpp"

#include "axion_engine/runtime/components/renderer/RenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/SpriteRenderComponent.hpp"

#include "axion_engine/runtime/components/collider/ColliderComponent.hpp"
#include "axion_engine/runtime/components/collider/SphereColliderComponent.hpp"

class RenderManager : public ContextAware
{
public:
    RenderManager(EngineContext &ctx);
    ~RenderManager();

    void Update();

private:
    std::vector<IRenderable *> renderables;
    void CollectRenderables(Scene& scene, std::vector<IRenderable*>& out);

    SDL_Renderer *renderer_;
};