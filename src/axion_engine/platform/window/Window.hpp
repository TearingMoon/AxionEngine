#pragma once

#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <memory>

#include "WindowConfig.hpp"
#include "axion_engine/structure/ContextAware.hpp"

class Window : public ContextAware
{
public:
    Window(EngineContext &ctx);
    ~Window() = default;

    void Start(const WindowConfig &config);

    void RestartWindow(const WindowConfig &config);

    SDL_Window *GetSDLWindow() const { return window_.get(); }
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
    SDL_Renderer *renderer_ = nullptr;

    bool initialized_ = false;

    bool Initialize(const WindowConfig &config);
    void Reset() noexcept;

    SDL_Window *CreateWindow(const WindowConfig &config) noexcept;
    SDL_Renderer *CreateRenderer(SDL_Window *window) noexcept;
};