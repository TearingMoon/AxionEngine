#pragma once

#include <SDL.h>
#include <SDL2/SDL_image.h>

#include "axion_engine/structure/ContextAware.hpp"

class SDLManager : public ContextAware
{
public:
    SDLManager(EngineContext &ctx);
    ~SDLManager();

    void InitSDL();
    void QuitSDL();

    bool IsSDLInitialized() const { return initialized_; }

private:
    bool initialized_ = false;
};