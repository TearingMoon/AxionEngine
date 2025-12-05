#pragma once

class Logger;

class TimeManager;
class InputManager;
class SceneManager;
class PhysicsManager;
class RenderManager;
class Window;
class AssetsManager;

struct EngineContext
{
    Logger *logger;
    TimeManager *time;
    InputManager *input;
    SceneManager *scene;
    PhysicsManager *physics;
    RenderManager *render;
    Window *window;
    AssetsManager *assets;
};