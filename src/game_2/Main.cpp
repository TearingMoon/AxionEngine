#include <SDL.h>
#undef main
#include <stdio.h>

#include "axion_engine/core/Engine.hpp"

#include "game_2/scenes/Game2Scene.hpp"

int main(int argc, char *args[])
{
    EngineConfig engineConfig;
    engineConfig.updateDelay = 16; // 16 Approx ~60 FPS

    WindowConfig windowConfig = {
        "Game 2 - Axion Engine",
        {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED},
        {1024, 768},
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};

    Engine engine(engineConfig, windowConfig);

    // Define and register scenes here
    engine.GetSceneManager()->RegisterScene<Game2Scene>("Game2Scene");
    engine.GetSceneManager()->RequestChange("Game2Scene");

    engine.Run();

    return 0;
}
