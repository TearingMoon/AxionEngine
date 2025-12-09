#pragma once

class Logger;
class Analyzer;

class TimeManager;
class InputManager;
class SceneManager;
class PhysicsManager;
class RenderManager;
class AssetsManager;
class Window;
class AssetsManager;

struct EngineContext
{
    Logger *logger;
    Analyzer *analyzer;
    TimeManager *time;
    InputManager *input;
    SceneManager *scene;
    PhysicsManager *physics;
    RenderManager *render;
    AssetsManager *assets;
    Window *window;
};