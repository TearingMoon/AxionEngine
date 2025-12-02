#include "SDLManager.hpp"

SDLManager::SDLManager()
{
    InitSDL();
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
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    int imgFlags = IMG_INIT_PNG;
    int initted = IMG_Init(imgFlags);
    if ((initted & imgFlags) != imgFlags)
    {
        printf("SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        return;
    }

    printf("SDL initialized successfully. \n");
    initialized_ = true;
}

void SDLManager::QuitSDL()
{
    if (initialized_)
    {
        SDL_Quit();
        printf("SDL quit successfully. \n");
        initialized_ = false;
    }
}
