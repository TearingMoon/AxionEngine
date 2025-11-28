#include "SDLManager.hpp"

SDLManager::SDLManager()
{
    InitSDL();
}

SDLManager::~SDLManager()
{
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

    // if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    // {
    //     printf("SDL image could not initialize! SDL_Error: %s\n", SDL_GetError());
    //     return;
    // }

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
