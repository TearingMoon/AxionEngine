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

SDL_Texture *Window::LoadTexture(SDL_Renderer *renderer, const std::string &path)
{
    SDL_Surface *surface = IMG_Load(path.c_str());
    if (!surface)
    {
        printf("IMG_Load failed: %s\n", IMG_GetError());
        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
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
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return nullptr;
    }

    return win;
}

SDL_Renderer *Window::CreateRenderer(SDL_Window *window) noexcept
{
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Could not create renderer! SDL_Error: %s\n", SDL_GetError());
        return nullptr;
    }

    return renderer;
}
