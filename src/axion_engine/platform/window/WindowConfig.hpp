#pragma once

#include <SDL.h>
#include <string>

#include "axion_data/Transform.hpp"

struct WindowConfig
{
    std::string title = "Axion Engine";
    Transform2DSimple transform = {{SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED}, {800, 600}, 0};
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    bool operator==(const WindowConfig &other) const
    {
        return title == other.title &&
               transform == other.transform &&
               flags == other.flags;
    }
};