#pragma once
#include <SDL2/SDL.h>
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"

namespace Axion
{

/**
 * @brief Context passed to renderable components during rendering.
 */
struct RenderContext
{
    SDL_Renderer* renderer = nullptr;
    CameraComponent* camera = nullptr;
    int windowWidth = 0;
    int windowHeight = 0;
};

/**
 * @brief Render layer ordering for draw calls.
 * 
 * Objects are rendered from lowest to highest layer value.
 */
enum class RenderLayer
{
    Background = 0,     ///< Rendered first (behind everything)
    Default = 100,      ///< Standard game objects
    Foreground = 200,   ///< Rendered above default objects
    Debug = 250,        ///< Debug visualization (F3 to toggle)
    UI = 300            ///< User interface (rendered last)
};

/**
 * @brief Interface for components that can be rendered.
 * 
 * Components implementing IRenderable are collected by the RenderManager
 * and drawn each frame according to their layer and order.
 */
class IRenderable
{
public:
    virtual ~IRenderable() = default;
    
    /** @brief Returns the render layer for sorting. */
    virtual RenderLayer GetLayer() const { return RenderLayer::Default; }
    
    /** @brief Returns the order within the layer (lower = rendered first). */
    virtual int GetOrderInLayer() const = 0;

    /**
     * @brief Renders the component.
     * @param ctx Rendering context with renderer and camera info
     */
    virtual void Render(const RenderContext& ctx) = 0;
};

} // namespace Axion