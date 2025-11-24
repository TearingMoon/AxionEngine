#include "InputManager.hpp"

InputManager::InputManager()
{
    currentKeys_.fill(0);
    previousKeys_.fill(0);
    printf("- InputManager initialized. \n");
}

InputManager::~InputManager()
{
    printf("- InputManager destroyed. \n");
}

#pragma region Fetch Inputs

void InputManager::BeginFrame()
{
    previousKeys_ = currentKeys_;
    previousMouseButtons_ = currentMouseButtons_;
}

void InputManager::ProcessEvent(const SDL_Event &e)
{
    switch (e.type)
    {
    case SDL_KEYDOWN:
        currentKeys_[e.key.keysym.scancode] = 1;
        break;

    case SDL_KEYUP:
        currentKeys_[e.key.keysym.scancode] = 0;
        break;

    case SDL_MOUSEBUTTONDOWN:
        currentMouseButtons_ |= SDL_BUTTON(e.button.button);
        break;

    case SDL_MOUSEBUTTONUP:
        currentMouseButtons_ &= ~SDL_BUTTON(e.button.button);
        break;

    case SDL_MOUSEMOTION:
        mousePosition_.x = e.motion.x;
        mousePosition_.y = e.motion.y;
        break;
    }
}

void InputManager::EndFrame()
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        currentKeys_[i] = state[i];

    int x, y;
    currentMouseButtons_ = SDL_GetMouseState(&x, &y);
    mousePosition_ = {x, y};
}

#pragma endregion

bool InputManager::IsKeyDown(SDL_Scancode key) const
{
    return currentKeys_[key] != 0;
}

bool InputManager::IsKeyJustPressed(SDL_Scancode key) const
{
    return currentKeys_[key] != 0 && previousKeys_[key] == 0;
}

bool InputManager::IsKeyJustReleased(SDL_Scancode key) const
{
    return currentKeys_[key] == 0 && previousKeys_[key] != 0;
}

bool InputManager::IsMouseButtonDown(Uint8 button) const
{
    return (currentMouseButtons_ & SDL_BUTTON(button)) != 0;
}

bool InputManager::IsMouseButtonJustPressed(Uint8 button) const
{
    bool now = (currentMouseButtons_ & SDL_BUTTON(button)) != 0;
    bool before = (previousMouseButtons_ & SDL_BUTTON(button)) != 0;
    return now && !before;
}

bool InputManager::IsMouseButtonJustReleased(Uint8 button) const
{
    bool now = (currentMouseButtons_ & SDL_BUTTON(button)) != 0;
    bool before = (previousMouseButtons_ & SDL_BUTTON(button)) != 0;
    return !now && before;
}
