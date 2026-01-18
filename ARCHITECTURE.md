# Modular Engine Architecture

## Description

The project has been restructured to support multiple games using the same engine as shared libraries.

## Components

### AxionEngine (Static Library)
- Contains all game engine code
- Compiled once and can be linked by multiple games
- Includes:
  - Scene system and GameObjects
  - Managers (Render, Physics, Input, etc.)
  - Components (Colliders, Cameras, Renderers, etc.)
  - SDL2 integration

### AxionUtilities (Header-only Library)
- Shared utilities that don't require separate compilation
- Includes helpers for path handling and vector filtering

### Game and Game2 (Executables)
- Each game is an independent executable
- Both link against AxionEngine and AxionUtilities
- Each game can have its own:
  - Assets
  - Scenes
  - Custom scripts

## Building

### Windows (vcpkg)

```bash
# Configure
cmake --preset windows-vcpkg-debug

# Build
cmake --build build/windows-debug --config Debug

# Executables will be in:
# build/windows-debug/bin/Game/Game.exe
# build/windows-debug/bin/Game2/Game2.exe
```

### Linux

```bash
# Configure
cmake --preset linux-debug

# Build
cmake --build build/linux-debug

# Executables will be in:
# build/linux-debug/bin/Game/Game
# build/linux-debug/bin/Game2/Game2
```

## Creating a New Game

To create a third game (game_3), follow these steps:

1. **Create the folder structure:**
   ```
   src/game_3/
   ├── Main.cpp
   ├── assets/
   └── scenes/
   ```

2. **Create Main.cpp:**
   ```cpp
   #include <SDL.h>
   #undef main
   #include <stdio.h>

   #include "axion_engine/core/Engine.hpp"
   #include "game_3/scenes/MyScene.hpp"

   int main(int argc, char *args[])
   {
       EngineConfig engineConfig;
       engineConfig.updateDelay = 16;

       WindowConfig windowConfig = {
           "My Game 3",
           {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED},
           {1280, 720},
           SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE};

       Engine engine(engineConfig, windowConfig);

       engine.GetSceneManager()->RegisterScene<MyScene>("MyScene");
       engine.GetSceneManager()->RequestChange("MyScene");

       engine.Run();

       return 0;
   }
   ```

3. **Add to CMakeLists.txt:**
   After the Game2 section, add:
   ```cmake
   # ======================================
   # Game 3 Executable
   # ======================================
   file(GLOB_RECURSE GAME3_SOURCES CONFIGURE_DEPENDS 
       "${CMAKE_SOURCE_DIR}/src/game_3/*.cpp"
       "${CMAKE_SOURCE_DIR}/src/game_3/**/*.cpp"
   )
   if(GAME3_SOURCES)
       add_executable(Game3 ${GAME3_SOURCES})

       target_include_directories(Game3 PRIVATE
           ${CMAKE_SOURCE_DIR}/src
       )

       target_link_libraries(Game3 PRIVATE
           AxionEngine
           AxionUtilities
       )

       if(WIN32)
           target_link_libraries(Game3 PRIVATE
               SDL2::SDL2main
           )
       endif()

       set_target_properties(Game3 PROPERTIES
           RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/Game3
           RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin/Game3
           RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Game3
       )

       set(GAME3_ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/game_3/assets)
       set(GAME3_ASSETS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/Game3/assets)
       add_custom_target(copy_game3_assets ALL
           COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${GAME3_ASSETS_SOURCE_DIR}
                   ${GAME3_ASSETS_OUTPUT_DIR}
           COMMENT "Copying Game3 assets to output directory"
       )
       add_dependencies(Game3 copy_game3_assets)
   endif()
   ```

4. **Reconfigure and build:**
   ```bash
   cmake --preset windows-vcpkg-debug
   cmake --build build/windows-debug --config Debug
   ```

## Advantages of This Architecture

1. **Code reusability:** The engine is compiled once and used in multiple games
2. **Parallel development:** Different teams can work on different games simultaneously
3. **Faster compilation:** Changes in one game don't require recompiling others
4. **Clear separation:** Each game has its own assets and specific code
5. **Easy extension:** Adding new games is straightforward following the template

## Important Notes

- **Windows**: It's necessary to include `#undef main` after `#include <SDL.h>` in Main.cpp
- **Assets**: Each game has its own assets folder in the output directory
- **Dependencies**: All games share the same versions of SDL2 and other dependencies
