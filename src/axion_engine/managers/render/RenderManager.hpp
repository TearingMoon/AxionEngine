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

#include "axion_engine/runtime/components/collider/SphereColliderComponent.hpp"

class RenderManager : public ContextAware
{
public:
    RenderManager(EngineContext &ctx);
    ~RenderManager();

    void Update();

private:
    void CollectRenderItems(std::vector<RenderItem> &outItems);
    void SortRenderItems(std::vector<RenderItem> &items);
    void DrawItem(CameraComponent &camera, const RenderItem &item);

    void DrawColliders(CameraComponent &camera);
    void DrawSphereCollider(
        const SphereColliderComponent &sphere,
        const TransformComponent &transform,
        CameraComponent &camera,
        const glm::vec3 &camPos,
        int winW, int winH);
    void DrawCircle(int centerX, int centerY, int radius);

    glm::vec2 WorldToScreen(const glm::vec3 &worldPos, const glm::vec3 &camPos, int winW, int winH, float zoom = 1.0f);

    SDL_Renderer *renderer_;
};