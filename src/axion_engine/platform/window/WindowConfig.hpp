#pragma once

#include <SDL.h>
#include <glm/glm.hpp>
#include <string>

struct WindowConfig
{
    std::string title = "Axion Engine";

    glm::ivec2 position = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED};
    glm::ivec2 size = {800, 600};

    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    bool operator==(const WindowConfig &other) const
    {
        return position == other.position &&
               size == other.size &&
               title == other.title &&
               flags == other.flags;
    }
};