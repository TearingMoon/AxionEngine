#pragma once

class TimeManager;
class InputManager;
class SceneManager;
class PhysicsManager;
class RenderManager;

struct EngineContext
{
    TimeManager *time;
    InputManager *input;
    SceneManager *scene;
    PhysicsManager *physics;
    RenderManager *render;
};