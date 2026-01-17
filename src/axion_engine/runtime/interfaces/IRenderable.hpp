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

enum class RenderLayer
{
    Background = 0,
    Default = 100,
    Foreground = 200,
    Debug = 250,
    UI = 300
};

class IRenderable
{
public:
    virtual ~IRenderable() = default;
    virtual RenderLayer GetLayer() const { return RenderLayer::Default; }
    virtual int GetSortKey() const = 0;
    virtual int GetOrderInLayer() const = 0;

    virtual void Render(const RenderContext &ctx) = 0;
};