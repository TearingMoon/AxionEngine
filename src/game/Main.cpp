#include <SDL.h>
#include <stdio.h>

#include "axion_engine/core/Engine.hpp"

#include "game/scenes/MainMenuScene.hpp"

// #include "engine/core/AxionEngine.hpp"

int main(int argc, char *args[])
{

    EngineConfig engineConfig;
    engineConfig.updateDelay = 16; // 16 Approx ~60 FPS

    WindowConfig windowConfig = {
        "Axion Engine Window",
        {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED},
        {800, 600},
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};

    Engine engine(engineConfig, windowConfig);

    // TODO: Make the window instantiation here

    // Define and register scenes here
    engine.GetSceneManager()->RegisterScene<MainMenuScene>("MainMenu");
    engine.GetSceneManager()->RequestChange("MainMenu");

    engine.Run();

    return 0;
}