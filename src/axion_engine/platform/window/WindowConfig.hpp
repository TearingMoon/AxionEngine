#pragma once

#include <SDL.h>
#include <glm/glm.hpp>
#include <string>

namespace Axion
{

/**
 * @brief Configuration structure for window creation.
 * 
 * Contains all parameters needed to initialize the application window
 * including size, position, title, and SDL window flags.
 */
struct WindowConfig
{
    /** @brief Window title displayed in the title bar. */
    std::string title = "Axion Engine";

    /** @brief Initial window position. Use SDL_WINDOWPOS_CENTERED for centering. */
    glm::ivec2 position = {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED};
    
    /** @brief Initial window dimensions in pixels. */
    glm::ivec2 size = {800, 600};

    /** @brief SDL window creation flags (SDL_WINDOW_SHOWN, SDL_WINDOW_RESIZABLE, etc.). */
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    bool operator==(const WindowConfig& other) const
    {
        return position == other.position &&
               size == other.size &&
               title == other.title &&
               flags == other.flags;
    }
};

} // namespace Axion