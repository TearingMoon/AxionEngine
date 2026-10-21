#include "Window.hpp"

namespace Axion
{

Window::Window(EngineContext& ctx) : ContextAware(ctx)
{
}

void Window::Start(const WindowConfig &config)
{
    AXION_INFO("Creating window: {}", config.title.c_str());
    if (Initialize(config))
    {
        AXION_INFO("Window created successfully.");
    }
    else
    {
        AXION_ERROR("Failed to create window.");
    }
}

void Window::RestartWindow(const WindowConfig &config)
{
    AXION_INFO("Restarting window: {}", config.title.c_str());
    Reset();
    if (Initialize(config))
    {
        AXION_INFO("Window restarted successfully.");
    }
    else
    {
        AXION_ERROR("Failed to restart window.");
    }
}

bool Window::Initialize(const WindowConfig &config)
{
    auto newWindow = CreateWindow(config);
    if (!newWindow)
    {
        Reset();
        return false;
    }

    auto newRenderer = CreateRenderer(newWindow);
    if (!newRenderer)
    {
        Reset();
        return false;
    }

    // Set the new window, surface and renderer
    window_.reset(newWindow);
    renderer_ = newRenderer;

    // Placeholder surface
    SDL_UpdateWindowSurface(window_.get());

    initialized_ = true;
    return true;
}

void Window::Reset() noexcept
{
    if (renderer_)
    {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    window_.reset();
    initialized_ = false;
}

SDL_Window *Window::CreateWindow(const WindowConfig &config) noexcept
{
    SDL_Window *win = SDL_CreateWindow(
        config.title.c_str(),
        config.position.x,
        config.position.y,
        config.size.x,
        config.size.y,
        config.flags);

    if (!win)
    {
        AXION_ERROR("Window could not be created! SDL_Error: {}", SDL_GetError());
        return nullptr;
    }

    // Hide mouse cursor when over the window
    SDL_ShowCursor(SDL_DISABLE);

    return win;
}

SDL_Renderer *Window::CreateRenderer(SDL_Window *window) noexcept
{
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        AXION_ERROR("Could not create renderer! SDL_Error: {}", SDL_GetError());
        return nullptr;
    }

    return renderer;
}

} // namespace Axion
