# Arquitectura Modular del Motor

## Descripción

El proyecto ha sido reestructurado para soportar múltiples juegos usando el mismo motor como librerías compartidas.

## Estructura del Proyecto

```
GameEngine/
├── src/
│   ├── axion_engine/       # Motor de juego (Librería estática)
│   │   ├── core/
│   │   ├── managers/
│   │   ├── platform/
│   │   └── runtime/
│   │
│   ├── axion_utilities/    # Utilidades compartidas (Librería de solo headers)
│   │   ├── path/
│   │   └── vector_filter/
│   │
│   ├── game/               # Juego 1 (Ejecutable)
│   │   ├── Main.cpp
│   │   ├── assets/
│   │   ├── scenes/
│   │   └── scripts/
│   │
│   └── game_2/             # Juego 2 (Ejecutable)
│       ├── Main.cpp
│       ├── assets/
│       └── scenes/
│
└── build/
    └── windows-debug/
        └── bin/
            ├── Game.exe        # Ejecutable del juego 1
            ├── Game2.exe       # Ejecutable del juego 2
            ├── assets/         # Assets del juego 1
            └── assets_game2/   # Assets del juego 2
```

## Componentes

### AxionEngine (Librería Estática)
- Contiene todo el código del motor de juego
- Se compila una vez y puede ser linkeada por múltiples juegos
- Incluye:
  - Sistema de escenas y GameObjects
  - Managers (Render, Physics, Input, etc.)
  - Componentes (Colliders, Cameras, Renderers, etc.)
  - Integración con SDL2

### AxionUtilities (Librería de Headers)
- Utilidades compartidas que no requieren compilación separada
- Incluye helpers para manejo de rutas y filtrado de vectores

### Game y Game2 (Ejecutables)
- Cada juego es un ejecutable independiente
- Ambos linkean contra AxionEngine y AxionUtilities
- Cada juego puede tener sus propios:
  - Assets
  - Escenas
  - Scripts personalizados

## Compilación

### Windows (vcpkg)

```bash
# Configurar
cmake --preset windows-vcpkg-debug

# Compilar
cmake --build build/windows-debug --config Debug

# Los ejecutables estarán en:
# build/windows-debug/bin/Game.exe
# build/windows-debug/bin/Game2.exe
```

### Linux

```bash
# Configurar
cmake --preset linux-debug

# Compilar
cmake --build build/linux-debug

# Los ejecutables estarán en:
# build/linux-debug/bin/Game
# build/linux-debug/bin/Game2
```

## Crear un Nuevo Juego

Para crear un tercer juego (game_3), sigue estos pasos:

1. **Crear la estructura de carpetas:**
   ```
   src/game_3/
   ├── Main.cpp
   ├── assets/
   └── scenes/
   ```

2. **Crear Main.cpp:**
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

3. **Agregar al CMakeLists.txt:**
   Después de la sección de Game2, agrega:
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
           RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
           RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR}/bin
           RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
       )

       set(GAME3_ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/game_3/assets)
       set(GAME3_ASSETS_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin/assets_game3)
       add_custom_target(copy_game3_assets ALL
           COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${GAME3_ASSETS_SOURCE_DIR}
                   ${GAME3_ASSETS_OUTPUT_DIR}
           COMMENT "Copying Game3 assets to output directory"
       )
       add_dependencies(Game3 copy_game3_assets)
   endif()
   ```

4. **Reconfigurar y compilar:**
   ```bash
   cmake --preset windows-vcpkg-debug
   cmake --build build/windows-debug --config Debug
   ```

## Ventajas de Esta Arquitectura

1. **Reutilización de código:** El motor se compila una vez y se usa en múltiples juegos
2. **Desarrollo paralelo:** Diferentes equipos pueden trabajar en diferentes juegos simultáneamente
3. **Compilación más rápida:** Cambios en un juego no requieren recompilar otros
4. **Separación clara:** Cada juego tiene sus propios assets y código específico
5. **Fácil extensión:** Agregar nuevos juegos es straightforward siguiendo la plantilla

## Notas Importantes

- **Windows**: Es necesario incluir `#undef main` después de `#include <SDL.h>` en Main.cpp
- **Assets**: Cada juego tiene su propia carpeta de assets en el directorio de salida
- **Dependencias**: Todos los juegos comparten las mismas versiones de SDL2 y otras dependencias
