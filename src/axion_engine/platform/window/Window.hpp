#pragma once

#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>

#include "WindowConfig.hpp"
#include "axion_engine/structure/ContextAware.hpp"

namespace Axion
{

/**
 * @brief Manages the application window and its associated renderer.
 * 
 * Window encapsulates SDL window creation, management, and destruction.
 * It provides access to the underlying SDL_Window and SDL_Renderer for
 * rendering operations.
 */
class Window : public ContextAware
{
public:
    /**
     * @brief Constructs a Window object.
     * @param ctx Reference to the engine context
     */
    Window(EngineContext& ctx);
    ~Window() = default;

    /**
     * @brief Creates and displays the window with the specified configuration.
     * @param config Window configuration parameters
     */
    void Start(const WindowConfig& config);

    /**
     * @brief Destroys and recreates the window with new settings.
     * @param config New window configuration
     */
    void RestartWindow(const WindowConfig& config);

    /** @brief Returns the underlying SDL window handle. */
    SDL_Window* GetSDLWindow() const { return window_.get(); }
    
    /** @brief Returns the SDL renderer for this window. */
    SDL_Renderer* GetRenderer() const { return renderer_; }

private:
    struct SDL_WindowDeleter
    {
        void operator()(SDL_Window* ptr) const noexcept
        {
            if (ptr)
                SDL_DestroyWindow(ptr);
        }
    };
    
    std::unique_ptr<SDL_Window, SDL_WindowDeleter> window_{nullptr};
    SDL_Renderer* renderer_ = nullptr;
    bool initialized_ = false;

    bool Initialize(const WindowConfig& config);
    void Reset() noexcept;
    SDL_Window* CreateWindow(const WindowConfig& config) noexcept;
    SDL_Renderer* CreateRenderer(SDL_Window* window) noexcept;
};

} // namespace Axion