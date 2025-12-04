#pragma once

class TimeManager;
class InputManager;
class SceneManager;
class PhysicsManager;
class RenderManager;
class AssetsManager;
class Window;

struct EngineContext
{
    TimeManager *time;
    InputManager *input;
    SceneManager *scene;
    PhysicsManager *physics;
    RenderManager *render;
    AssetsManager *assets;
    Window *window;
};