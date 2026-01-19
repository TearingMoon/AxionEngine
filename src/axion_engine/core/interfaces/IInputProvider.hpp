#pragma once
#include <SDL.h>

namespace Axion
{

/**
 * @brief Interface for input queries.
 * 
 * Provides read-only access to input state without
 * exposing the full InputManager implementation.
 */
class IInputProvider
{
public:
    virtual ~IInputProvider() = default;
    
    /** @brief Returns true if the key is currently held down. */
    virtual bool IsKeyDown(SDL_Scancode key) const = 0;
    
    /** @brief Returns true if the key was just pressed this frame. */
    virtual bool IsKeyJustPressed(SDL_Scancode key) const = 0;
    
    /** @brief Returns true if the key was just released this frame. */
    virtual bool IsKeyJustReleased(SDL_Scancode key) const = 0;
    
    /** @brief Returns true if the mouse button is currently held down. */
    virtual bool IsMouseButtonDown(Uint8 button) const = 0;
    
    /** @brief Returns true if the mouse button was just pressed this frame. */
    virtual bool IsMouseButtonJustPressed(Uint8 button) const = 0;
    
    /** @brief Returns the current mouse position in screen coordinates. */
    virtual SDL_Point GetMousePosition() const = 0;
};

} // namespace Axion
