#pragma once

#include <SDL.h>
#include <memory>

#include "WindowConfig.hpp"

class Window
{
public:
    Window(const WindowConfig &config);
    ~Window() = default;

    void RestartWindow(const WindowConfig &config);

private:
    struct SDL_WindowDeleter
    {
        void operator()(SDL_Window *ptr) const noexcept
        {
            if (ptr)
                SDL_DestroyWindow(ptr);
        }
    };
    std::unique_ptr<SDL_Window, SDL_WindowDeleter> window_ {nullptr};
    SDL_Surface *surface_ = nullptr;
    bool initialized_ = false;

    bool Initialize(const WindowConfig &config);
    void Reset() noexcept;

    static SDL_Window *CreateWindow(const WindowConfig &config) noexcept;
    static SDL_Surface *GetWindowSurface(SDL_Window *window) noexcept;
};