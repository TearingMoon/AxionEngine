#pragma once

#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>

#include "WindowConfig.hpp"

class Window
{
public:
    Window(const WindowConfig &config);
    ~Window() = default;

    void RestartWindow(const WindowConfig &config);

    SDL_Texture *LoadTexture(SDL_Renderer *renderer, const std::string &path);

    SDL_Window *GetSDLWindow() const { return window_.get(); }
    SDL_Surface *GetSurface() const { return surface_; }
    SDL_Renderer *GetRenderer() const { return renderer_; }

private:
    struct SDL_WindowDeleter
    {
        void operator()(SDL_Window *ptr) const noexcept
        {
            if (ptr)
                SDL_DestroyWindow(ptr);
        }
    };
    std::unique_ptr<SDL_Window, SDL_WindowDeleter> window_{nullptr};
    SDL_Surface *surface_ = nullptr;
    SDL_Renderer *renderer_ = nullptr;

    bool initialized_ = false;

    bool Initialize(const WindowConfig &config);
    void Reset() noexcept;

    static SDL_Window *CreateWindow(const WindowConfig &config) noexcept;
    static SDL_Surface *GetWindowSurface(SDL_Window *window) noexcept;
    static SDL_Renderer *GetWindowRenderer(SDL_Window *window) noexcept;
};