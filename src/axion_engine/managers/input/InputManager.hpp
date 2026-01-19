#pragma once
#include <stdio.h>
#include <SDL.h>
#include <array>

#include "axion_engine/structure/ContextAware.hpp"

namespace Axion
{

/**
 * @brief Handles keyboard and mouse input polling.
 * 
 * InputManager tracks the state of all keyboard keys and mouse buttons,
 * providing methods to check for key presses, releases, and holds.
 * Input state is updated each frame via BeginFrame/EndFrame.
 */
class InputManager : public ContextAware
{
public:
    InputManager(EngineContext& ctx);
    ~InputManager();
    
    /** @brief Prepares for a new frame of input processing. */
    void BeginFrame();
    
    /** @brief Processes an SDL event for input state updates. */
    void ProcessEvent(const SDL_Event& e);
    
    /** @brief Finalizes input state for the current frame. */
    void EndFrame();

    /** @brief Returns true if the key is currently held down. */
    bool IsKeyDown(SDL_Scancode key) const;
    
    /** @brief Returns true if the key was pressed this frame. */
    bool IsKeyJustPressed(SDL_Scancode key) const;
    
    /** @brief Returns true if the key was released this frame. */
    bool IsKeyJustReleased(SDL_Scancode key) const;

    /** @brief Returns true if the mouse button is currently held down. */
    bool IsMouseButtonDown(Uint8 button) const;
    
    /** @brief Returns true if the mouse button was pressed this frame. */
    bool IsMouseButtonJustPressed(Uint8 button) const;
    
    /** @brief Returns true if the mouse button was released this frame. */
    bool IsMouseButtonJustReleased(Uint8 button) const;

    /** @brief Returns the current mouse position in window coordinates. */
    SDL_Point GetMousePosition() const { return mousePosition_; }

    /** @brief Returns true if a quit event was received. */
    bool QuitRequested() const { return quitRequested_; }

private:
    std::array<Uint8, SDL_NUM_SCANCODES> currentKeys_{};
    std::array<Uint8, SDL_NUM_SCANCODES> previousKeys_{};

    Uint32 currentMouseButtons_ = 0;
    Uint32 previousMouseButtons_ = 0;
    SDL_Point mousePosition_{0, 0};

    bool quitRequested_ = false;
};

} // namespace Axion