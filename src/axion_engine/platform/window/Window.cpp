#include "Window.hpp"

Window::Window(const WindowConfig &config)
{
    printf("Creating window: %s\n", config.title.c_str());
    if (Initialize(config))
    {
        printf("- Window created successfully.\n");
    }
    else
    {
        printf("- Failed to create window.\n");
    }
}

void Window::RestartWindow(const WindowConfig &config)
{
    printf("Restarting window: %s\n", config.title.c_str());
    Reset();
    if (Initialize(config))
    {
        printf("Window restarted successfully.\n");
    }
    else
    {
        printf("Failed to restart window.\n");
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

    auto newSurface = GetWindowSurface(newWindow);
    if (!newSurface)
    {
        Reset();
        return false;
    }

    // Set the new window and surface
    window_.reset(newWindow);
    surface_ = newSurface;

    // Placeholder surface
    SDL_FillRect(surface_, nullptr, SDL_MapRGB(surface_->format, 0, 0, 0));
    SDL_UpdateWindowSurface(window_.get());

    initialized_ = true;
    return true;
}

void Window::Reset() noexcept
{
    window_.reset();
    surface_ = nullptr;
    initialized_ = false;
}

SDL_Window *Window::CreateWindow(const WindowConfig &config) noexcept
{
    SDL_Window *win = SDL_CreateWindow(
        config.title.c_str(),
        config.transform.position.x,
        config.transform.position.y,
        config.transform.scale.x,
        config.transform.scale.y,
        config.flags);

    if (!win)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return nullptr;
    }

    return win;
}

SDL_Surface *Window::GetWindowSurface(SDL_Window *window) noexcept
{
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (!surface)
        printf("Could not get window surface! SDL_Error: %s\n", SDL_GetError());

    return surface;
}