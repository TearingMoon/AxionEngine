#pragma once

#include <SDL.h>

class SDLManager
{
public:
    SDLManager();
    ~SDLManager();

    void InitSDL();
    void QuitSDL();

    bool IsSDLInitialized() const { return initialized_; }

private:
    bool initialized_ = false;
};