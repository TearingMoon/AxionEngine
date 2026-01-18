#include <SDL.h>
#undef main
#include <stdio.h>

#include "axion_engine/core/Engine.hpp"

#include "game/scenes/PongScene.hpp"

int main(int argc, char *args[])
{
    EngineConfig engineConfig;
    engineConfig.updateDelay = 16; // 16 Approx ~60 FPS

    WindowConfig windowConfig = {
        "Axion Pong - Engine Demo",
        {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED},
        {800, 600},
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};

    Engine engine(engineConfig, windowConfig);

    // Register Pong scene
    engine.GetSceneManager()->RegisterScene<PongScene>("Pong");
    engine.GetSceneManager()->RequestChange("Pong");

    engine.Run();

    return 0;
}