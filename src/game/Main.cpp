#include <SDL.h>
#include <stdio.h>

#include "axion_engine/core/Engine.hpp"

#include "game/scenes/MainMenuScene.hpp"

// #include "engine/core/AxionEngine.hpp"

int main(int argc, char *args[])
{

    EngineConfig engineConfig;
    engineConfig.updateDelay = 16; // Approx ~60 FPS

    WindowConfig windowConfig;
    windowConfig.title = "Axion Engine Window";
    windowConfig.transform.scale.x = 800;
    windowConfig.transform.scale.y = 600;

    Engine engine(engineConfig, windowConfig);

    // TODO: Make the window instantiation here

    // Define and register scenes here
    engine.GetSceneManager()->RegisterScene<MainMenuScene>("MainMenu");
    engine.GetSceneManager()->RequestChange("MainMenu");

    engine.Run();

    return 0;
}