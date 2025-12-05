#include "Window.hpp"

Window::Window(EngineContext &ctx) : ContextAware(ctx)
{
}

void Window::Start(const WindowConfig &config)
{
    INFO("Creating window: {}", config.title.c_str());
    if (Initialize(config))
    {
        INFO("Window created successfully.");
    }
    else
    {
        ERROR("Failed to create window.");
    }
}

void Window::RestartWindow(const WindowConfig &config)
{
    INFO("Restarting window: {}", config.title.c_str());
    Reset();
    if (Initialize(config))
    {
        INFO("Window restarted successfully.");
    }
    else
    {
        ERROR("Failed to restart window.");
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
    window_.reset();
    renderer_ = nullptr;
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
        ERROR("Window could not be created! SDL_Error: {}", SDL_GetError());
        return nullptr;
    }

    return win;
}

SDL_Renderer *Window::CreateRenderer(SDL_Window *window) noexcept
{
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        ERROR("Could not create renderer! SDL_Error: {}", SDL_GetError());
        return nullptr;
    }

    return renderer;
}
