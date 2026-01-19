#pragma once

#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "axion_engine/structure/ContextAware.hpp"

namespace Axion
{

/**
 * @brief Manages SDL library initialization and shutdown.
 * 
 * SDLManager handles the lifecycle of SDL, SDL_image, and SDL_ttf libraries.
 * It ensures proper initialization order and cleanup on destruction.
 */
class SDLManager : public ContextAware
{
public:
    SDLManager(EngineContext& ctx);
    ~SDLManager();

    /** @brief Initializes SDL and related libraries. */
    void InitSDL();
    
    /** @brief Shuts down SDL and releases resources. */
    void QuitSDL();

    /** @brief Returns whether SDL has been successfully initialized. */
    bool IsSDLInitialized() const { return initialized_; }

private:
    bool initialized_ = false;
};

} // namespace Axion