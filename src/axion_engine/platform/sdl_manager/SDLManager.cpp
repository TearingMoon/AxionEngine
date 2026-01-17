#include "SDLManager.hpp"

SDLManager::SDLManager(EngineContext &ctx) : ContextAware(ctx)
{
}

SDLManager::~SDLManager()
{
    IMG_Quit();
    QuitSDL();
}

void SDLManager::InitSDL()
{
    if (initialized_)
        return;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0)
    {
        ERROR("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    int imgFlags = IMG_INIT_PNG;
    int initted = IMG_Init(imgFlags);
    if ((initted & imgFlags) != imgFlags)
    {
        ERROR("SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        return;
    }

    if (TTF_Init() == -1)
    {
        ERROR("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return;
    }

    INFO("SDL initialized successfully.");
    initialized_ = true;
}

void SDLManager::QuitSDL()
{
    if (initialized_)
    {
        SDL_Quit();
        INFO("SDL quit successfully.");
        initialized_ = false;
    }
}
