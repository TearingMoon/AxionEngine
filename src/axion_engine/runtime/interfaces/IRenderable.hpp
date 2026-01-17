#pragma once
#include <SDL2/SDL.h>
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"

struct RenderContext
{
    SDL_Renderer *sdl = nullptr;
    CameraComponent *camera = nullptr;

    int windowWidth = 0;
    int windowHeight = 0;

    // add: camera, viewport, dt, etc.
};

class IRenderable
{
public:
    virtual ~IRenderable() = default;
    virtual int GetLayer() const = 0;
    virtual int GetSortKey() const = 0;
    virtual int GetOrderInLayer() const = 0;

    virtual void Render(const RenderContext &ctx) = 0;
};