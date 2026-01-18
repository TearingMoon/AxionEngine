# Axion Engine
Axion engine is a SDL2-based C++ multiplatform game engine for game development.

## Project Structure

This project uses a **modular architecture** where the engine is compiled as a library and can be used by multiple independent game executables. See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed information about the project structure.

### Components
- **AxionEngine**: Core game engine library (static)
- **AxionUtilities**: Shared utilities library (header-only)
- **Game**: Example game 1 executable
- **Game2**: Example game 2 executable

## Requirements

### Linux
- SDL2
- SDL2_image
- GLM
- Ninja Builder
- CMake (>= 3.16)

### Windows
- SDL2
- SDL2_image
- SDL2_ttf
- GLM
- CMake (>= 3.16)
- vcpkg

## Building the Project

### Windows (with vcpkg)
```bash
# Configure
cmake --preset windows-vcpkg-debug

# Build
cmake --build build/windows-debug --config Debug

# Executables will be in separate folders:
# - build/windows-debug/bin/Game/Game.exe
# - build/windows-debug/bin/Game2/Game2.exe
```

### Linux
```bash
# Configure
cmake --preset linux-debug

# Build
cmake --build build/linux-debug

# Executables will be in separate folders:
# - build/linux-debug/bin/Game/Game
# - build/linux-debug/bin/Game2/Game2
```

## Creating a New Game

To create a new game using the Axion Engine, refer to the detailed guide in [ARCHITECTURE.md](ARCHITECTURE.md#crear-un-nuevo-juego).
