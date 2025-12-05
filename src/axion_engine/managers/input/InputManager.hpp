#pragma once
#include <stdio.h>
#include <SDL.h>
#include <array>

#include "axion_engine/structure/ContextAware.hpp"

class InputManager : public ContextAware
{
public:
    InputManager(EngineContext &ctx);
    ~InputManager();
    
    void BeginFrame();
    void ProcessEvent(const SDL_Event& e);
    void EndFrame();

    bool IsKeyDown(SDL_Scancode key) const;
    bool IsKeyJustPressed(SDL_Scancode key) const;
    bool IsKeyJustReleased(SDL_Scancode key) const;

    bool IsMouseButtonDown(Uint8 button) const;
    bool IsMouseButtonJustPressed(Uint8 button) const;
    bool IsMouseButtonJustReleased(Uint8 button) const;

    SDL_Point GetMousePosition() const { return mousePosition_; }

    bool QuitRequested() const { return quitRequested; }

private:
    std::array<Uint8, SDL_NUM_SCANCODES> currentKeys_{};
    std::array<Uint8, SDL_NUM_SCANCODES> previousKeys_{};

    Uint32 currentMouseButtons_ = 0;
    Uint32 previousMouseButtons_ = 0;
    SDL_Point mousePosition_{0, 0};

    bool quitRequested = false;
};