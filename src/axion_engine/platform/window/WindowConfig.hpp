#pragma once

#include <SDL.h>
#include <string>

#include "axion_data/Transform.hpp"

struct WindowConfig
{
    std::string title = "Axion Engine";
    Transform2DSimple transform = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600};
    Uint32 flags = 0;

    bool operator==(const WindowConfig &other) const
    {
        return title == other.title &&
               transform == other.transform &&
               flags == other.flags;
    }
};