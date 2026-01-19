# Axion Engine - Architecture Documentation

## Overview

**Axion Engine** is a modular 2D game engine built in C++20 using SDL2 for rendering and input handling. The architecture follows a component-based design pattern where games are built by composing GameObjects with reusable Components.

The project is structured to support multiple independent games sharing the same engine core, promoting code reusability and parallel development.

---

## Table of Contents

1. [Project Structure](#project-structure)
2. [Core Architecture](#core-architecture)
3. [Engine Subsystems](#engine-subsystems)
4. [Runtime System](#runtime-system)
5. [Design Patterns](#design-patterns)
6. [Building](#building)
7. [Creating a New Game](#creating-a-new-game)
8. [API Reference](#api-reference)

---

## Project Structure

```
GameEngine/
├── src/
│   ├── axion_engine/           # Core engine library
│   │   ├── core/               # Engine core (Engine, Context, Events)
│   │   ├── managers/           # Subsystem managers
│   │   ├── platform/           # SDL abstraction layer
│   │   ├── runtime/            # Runtime classes (Scene, GameObject, Components)
│   │   └── structure/          # Base structures (ContextAware)
│   │
│   ├── axion_utilities/        # Header-only utility library
│   │   ├── path/               # Path utilities (PathFinder)
│   │   └── vector_filter/      # Vector filtering utilities
│   │
│   ├── pong/                   # Example game: Pong
│   ├── twin_stick_shooter/     # Example game: Twin Stick Shooter
│   ├── game_debug/             # Debug/testing game
│   └── game_launcher/          # Game launcher utility
│
├── build/                      # CMake build directory
├── tests/                      # Unit tests
├── CMakeLists.txt              # Main build configuration
└── CMakePresets.json           # Build presets for Windows/Linux
```

---

## Core Architecture

### High-Level Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                            GAME APPLICATION                         │
│  ┌─────────────────────────────────────────────────────────────────┐│
│  │                         Main.cpp                                ││
│  │    Creates Engine → Registers Scenes → Runs Game Loop           ││
│  └─────────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────────┐
│                           AXION ENGINE                              │
│  ┌─────────────────────────────────────────────────────────────────┐│
│  │                         Engine                                  ││
│  │  ┌─────────────┐  ┌─────────────────────────────────────────┐   ││
│  │  │EngineConfig │  │            EngineContext                │   ││
│  │  └─────────────┘  │  (Service Locator for all subsystems)   │   ││
│  │                   └─────────────────────────────────────────┘   ││
│  └─────────────────────────────────────────────────────────────────┘│
│                                    │                                │
│         ┌──────────────────────────┼──────────────────────────┐     │
│         ▼                          ▼                          ▼     │
│  ┌─────────────┐           ┌─────────────┐           ┌─────────────┐│
│  │  MANAGERS   │           │   PLATFORM  │           │   RUNTIME   ││
│  ├─────────────┤           ├─────────────┤           ├─────────────┤│
│  │SceneManager │           │ SDLManager  │           │   Scene     ││
│  │RenderManager│           │   Window    │           │ GameObject  ││
│  │PhysicsManager│          └─────────────┘           │ Components  ││
│  │InputManager │                                     └─────────────┘│
│  │TimeManager  │                                                    │
│  │AssetsManager│                                                    │
│  │  Logger     │                                                    │
│  │ Analyzer    │                                                    │
│  │ EventBus    │                                                    │
│  └─────────────┘                                                    │
└─────────────────────────────────────────────────────────────────────┘
```

### Libraries

#### AxionEngine (Static Library)

The main engine library containing all core functionality:

| Module | Description |
|--------|-------------|
| `core/` | Engine initialization, configuration, context, events and interfaces |
| `managers/` | All subsystem managers (render, physics, input, scene, assets, time, logger, analysis) |
| `platform/` | SDL2 abstraction (window management, SDL initialization) |
| `runtime/` | Scene, GameObject, and all component types |
| `structure/` | Base classes like `ContextAware` |

**Dependencies:** SDL2, SDL2_image, SDL2_ttf, GLM

#### AxionUtilities (Header-only Library)

Lightweight utility functions with no compilation overhead:

- **PathFinder**: Cross-platform path resolution for assets
- **PtrVectorFilter**: Utility for filtering vectors of pointers by type

---

## Engine Subsystems

### Engine Class

The `Engine` class is the main entry point that orchestrates all subsystems:

```cpp
namespace Axion {
    class Engine {
    public:
        Engine(EngineConfig config, WindowConfig windowConfig);
        void Run();  // Starts the main game loop
        
        TimeManager* GetTimeManager() const;
        InputManager* GetInputManager() const;
        SceneManager* GetSceneManager() const;
    };
}
```

**Game Loop Flow:**
```
+--------------------------------------------------------------+
|                        MAIN GAME LOOP                        |
|                                                              |
|   +-----------+     +-----------+     +-----------+          |
|   | EventLoop |---->|  AppLoop  |---->|  Render   |---+      |
|   |(SDL Events)|    | (Update)  |     |  (Draw)   |   |      |
|   +-----------+     +-----------+     +-----------+   |      |
|        ^                                              |      |
|        +----------------------------------------------+      |
+--------------------------------------------------------------+
```

### EngineContext (Service Locator)

`EngineContext` provides centralized access to all subsystems:

```cpp
struct EngineContext {
    // Interface-based access (ISP compliant - for components)
    ITimeProvider* timeProvider;
    IInputProvider* inputProvider;
    IAssetProvider* assetProvider;
    ISceneProvider* sceneProvider;
    
    // Full manager access (for engine internals)
    Logger* logger;
    Analyzer* analyzer;
    TimeManager* time;
    InputManager* input;
    SceneManager* scene;
    PhysicsManager* physics;
    RenderManager* render;
    AssetsManager* assets;
    Window* window;
    EventBus* events;
};
```

### Manager Subsystems

#### TimeManager
Handles time tracking and delta time calculation:
- `GetDeltaTime()`: Time since last frame (seconds)
- `GetElapsedTime()`: Total time since engine start
- `GetFPS()`: Current frames per second

#### InputManager
Polls keyboard and mouse input state:
- `IsKeyDown(SDL_Scancode)`: Key currently held
- `IsKeyJustPressed(SDL_Scancode)`: Key pressed this frame
- `IsKeyJustReleased(SDL_Scancode)`: Key released this frame
- `IsMouseButtonDown(Uint8)`: Mouse button state
- `GetMousePosition()`: Current mouse coordinates

#### SceneManager
Manages scene lifecycle and transitions:
- `RegisterScene<T>(name)`: Register a scene type
- `RequestChange(name)`: Queue scene transition (safe during update)
- `GetCurrentScene()`: Access active scene

#### PhysicsManager
Fixed-timestep physics simulation:
- Collision detection (AABB, OBB, Circle)
- Collision event dispatch (Enter/Exit)
- Rigidbody force integration
- Gravity application

#### RenderManager
Renders all visible objects with layer sorting:

| Layer | Value | Description |
|-------|-------|-------------|
| Background | 0 | Behind everything |
| Default | 100 | Standard game objects |
| Foreground | 200 | In front of gameplay |
| Debug | 250 | Only visible in debug mode (F3) |
| UI | 300 | User interface elements |

#### AssetsManager
Texture loading and caching:
- `LoadTexture(id, path)`: Load and cache a texture
- `GetTexture(id)`: Retrieve cached texture
- `UnloadTexture(id)`: Free texture memory

#### Logger
File-based logging with rotation:
- `Info(fmt, ...)`: Informational messages
- `Warn(fmt, ...)`: Warning conditions
- `Error(fmt, ...)`: Error conditions

#### EventBus
Publish-subscribe event system for decoupled communication:
```cpp
// Subscribe
auto id = eventBus.Subscribe<PlayerDiedEvent>([](const PlayerDiedEvent& e) {
    // Handle event
});

// Publish
eventBus.Publish(PlayerDiedEvent{playerId});

// Unsubscribe
eventBus.Unsubscribe<PlayerDiedEvent>(id);
```

---

## Runtime System

### Scene

Scenes are containers for GameObjects representing game levels or screens:

```cpp
class MyScene : public Axion::Scene {
public:
    using Scene::Scene;
    
    void OnSceneEnter() override {
        // Called when scene becomes active
        auto player = CreateGameObject();
        player->AddComponent<PlayerController>();
    }
    
    void OnSceneUpdate() override {
        // Called every frame
    }
    
    void OnSceneExit() override {
        // Called when leaving scene
    }
};
```

**Scene Lifecycle:**
```
RegisterScene() --> RequestChange() --> OnSceneEnter() --> OnSceneUpdate() (loop)
                                                                   |
                                                                   v
                                       OnSceneExit() <-- RequestChange() (to another scene)
```

### GameObject

GameObjects are entity containers with a component-based architecture:

```cpp
// Create a GameObject
GameObject* player = scene->CreateGameObject();

// Transform is automatically included
player->GetTransform()->SetPosition({100, 200, 0});

// Add components
auto sprite = player->AddComponent<SpriteRenderComponent>();
auto collider = player->AddComponent<AABBColliderComponent>();
auto script = player->AddComponent<PlayerController>();

// Query components
auto renderer = player->GetComponent<RenderComponent>();

// Destroy
player->Destroy();  // Marked for destruction at frame end
```

**GameObject Lifecycle:**
```
CreateGameObject() --> Mounted() --> Tick() (every frame) --> Destroy() --> OnDestroy()
                          |                                       |
                          v                                       v
                   Component::OnMounted()               Component::OnDestroy()
```

### Components

Components add functionality to GameObjects through composition:

#### Base Component
```cpp
class Component {
public:
    virtual void OnMounted(EngineContext& ctx) {}
    virtual void OnDestroy(EngineContext& ctx) {}
    virtual void OnEnabled(EngineContext& ctx) {}
    virtual void OnDisabled(EngineContext& ctx) {}
    
    GameObject* GetOwner() const;
};
```

#### Available Components

| Category | Component | Description |
|----------|-----------|-------------|
| **Transform** | `TransformComponent` | Position, rotation, scale (auto-added to all GameObjects) |
| **Rendering** | `SpriteRenderComponent` | Texture/sprite rendering |
| | `SquareRenderComponent` | Colored rectangle rendering |
| | `CircleRenderComponent` | Colored circle rendering |
| | `TextRenderComponent` | Text rendering with SDL_ttf |
| **Physics** | `AABBColliderComponent` | Axis-aligned bounding box collision |
| | `OBBColliderComponent` | Oriented bounding box collision |
| | `CircleColliderComponent` | Circle collision |
| | `RigidBody` | Physics body with forces and velocity |
| **Camera** | `CameraComponent` | Viewport and camera control |
| **Scripting** | `ScriptableComponent` | Base for custom game logic |

#### ScriptableComponent (Custom Logic)

Create game-specific behavior by extending `ScriptableComponent`:

```cpp
class PlayerController : public Axion::ScriptableComponent {
public:
    void Start(Axion::EngineContext& ctx) override {
        // Initialize player
    }
    
    void Update(Axion::EngineContext& ctx) override {
        // Per-frame logic
        if (ctx.inputProvider->IsKeyDown(SDL_SCANCODE_W)) {
            // Move forward
        }
    }
    
    void FixedUpdate(Axion::EngineContext& ctx) override {
        // Fixed timestep physics updates
    }
    
    void OnCollisionEnter(Axion::GameObject& other) override {
        // Handle collision start
    }
    
    void OnCollisionExit(Axion::GameObject& other) override {
        // Handle collision end
    }
};
```

### Interfaces

The engine uses Interface Segregation Principle (ISP) for clean dependencies:

| Interface | Purpose | Implementor |
|-----------|---------|-------------|
| `ITimeProvider` | Time queries (read-only) | `TimeManager` |
| `IInputProvider` | Input queries (read-only) | `InputManager` |
| `IAssetProvider` | Asset loading | `AssetsManager` |
| `ISceneProvider` | Scene management | `SceneManager` |
| `IUpdateable` | Per-frame updates | Components |
| `IRenderable` | Rendering capability | Render components |
| `ICollisionListener` | Collision events | Scriptable components |

---

## Design Patterns

### Patterns Used

| Pattern | Usage |
|---------|-------|
| **Component** | GameObject composition with reusable components |
| **Service Locator** | `EngineContext` provides access to subsystems |
| **Observer/Pub-Sub** | `EventBus` for decoupled communication |
| **Template Method** | Scene lifecycle callbacks (`OnSceneEnter`, etc.) |
| **Factory** | `CreateGameObject()` for object creation |
| **Interface Segregation** | Providers (ITimeProvider, IInputProvider, etc.) |

### ContextAware Base Class

Classes needing engine access inherit from `ContextAware`:

```cpp
class ContextAware {
public:
    explicit ContextAware(EngineContext& ctx);
    
protected:
    EngineContext& ctx_;
    
    // Convenient accessors
    Window& window() const;
    RenderManager& render() const;
    InputManager& input() const;
    TimeManager& time() const;
    SceneManager& scene() const;
    PhysicsManager& physics() const;
    AssetsManager& assets() const;
    Logger& logger() const;
};

// Logging macros available in ContextAware classes
INFO("Player spawned at {}", position);
WARN("Low health: {}", health);
ERROR("Failed to load texture: {}", path);
```

---

## Building

### Prerequisites

- **C++20** compatible compiler (MSVC 19.29+, GCC 10+, Clang 11+)
- **CMake** 3.16+
- **vcpkg** (Windows) or system packages (Linux)

### Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| SDL2 | 2.x | Window, rendering, input |
| SDL2_image | 2.x | Image loading (PNG, JPG, etc.) |
| SDL2_ttf | 2.x | Font rendering |
| GLM | 0.9.9+ | Math library (vectors, matrices) |

### Windows (vcpkg)

```bash
# Configure
cmake --preset windows-vcpkg-debug

# Build all
cmake --build build/windows-debug --config Debug

# Build specific game
cmake --build build/windows-debug --config Debug --target Pong

# Executables location:
# build/windows-debug/bin/Pong/Pong.exe
# build/windows-debug/bin/TwinStickShooter/TwinStickShooter.exe
# build/windows-debug/bin/GameDebug/GameDebug.exe
```

### Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libglm-dev

# Configure
cmake --preset linux-debug

# Build
cmake --build build/linux-debug

# Executables location:
# build/linux-debug/bin/Pong/Pong
# build/linux-debug/bin/TwinStickShooter/TwinStickShooter
```

### Build Targets

| Target | Description |
|--------|-------------|
| `AxionEngine` | Core engine static library |
| `Pong` | Pong example game |
| `TwinStickShooter` | Twin-stick shooter example |
| `GameDebug` | Debug/testing game |
| `GameLauncher` | Game launcher utility |

---

## Creating a New Game

### Step 1: Create Folder Structure

```
src/my_game/
├── Main.cpp
├── game.json
├── assets/
│   └── (textures, fonts, etc.)
├── scenes/
│   └── MainScene.hpp
└── scripts/
    └── (custom components)
```

### Step 2: Create Main.cpp

```cpp
#include <SDL.h>
#undef main  // Required on Windows
#include <stdio.h>

#include "axion_engine/core/Engine.hpp"
#include "my_game/scenes/MainScene.hpp"

int main(int argc, char* args[])
{
    Axion::EngineConfig engineConfig;
    engineConfig.updateDelay = 16;  // ~60 FPS

    Axion::WindowConfig windowConfig = {
        "My Game",                              // Title
        {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED},  // Position
        {1280, 720},                            // Size
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE // Flags
    };

    Axion::Engine engine(engineConfig, windowConfig);

    engine.GetSceneManager()->RegisterScene<MainScene>("Main");
    engine.GetSceneManager()->RequestChange("Main");

    engine.Run();

    return 0;
}
```

### Step 3: Create a Scene

```cpp
// scenes/MainScene.hpp
#pragma once
#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/components/renderer/SpriteRenderComponent.hpp"
#include "my_game/scripts/PlayerController.hpp"

class MainScene : public Axion::Scene {
public:
    using Scene::Scene;
    
    void OnSceneEnter() override {
        // Load assets
        GetContext().assets->LoadTexture("player", "player.png");
        
        // Create player
        auto player = CreateGameObject();
        player->GetTransform()->SetPosition({640, 360, 0});
        
        auto sprite = player->AddComponent<Axion::SpriteRenderComponent>();
        sprite->SetTexture(GetContext().assets->GetTexture("player"));
        
        player->AddComponent<PlayerController>();
        
        // Set up camera
        auto cameraObj = CreateGameObject();
        auto camera = cameraObj->AddComponent<Axion::CameraComponent>();
        SetCurrentCamera(camera);
    }
};
```

### Step 4: Create game.json

```json
{
    "name": "My Game",
    "version": "1.0.0",
    "author": "Your Name",
    "description": "A game made with Axion Engine"
}
```

### Step 5: Register in CMakeLists.txt

Add a single line to the Games section:

```cmake
# ======================================
# Games
# ======================================
axion_add_game(Pong pong)
axion_add_game(TwinStickShooter twin_stick_shooter)
axion_add_game(GameDebug game_debug)
axion_add_game(MyGame my_game)  # Add this line
```

### Step 6: Build

```bash
cmake --preset windows-vcpkg-debug
cmake --build build/windows-debug --config Debug --target MyGame
```

---

## API Reference

### Quick Reference

```cpp
// Engine
Axion::Engine engine(config, windowConfig);
engine.Run();
engine.GetSceneManager()->RegisterScene<T>("name");
engine.GetSceneManager()->RequestChange("name");

// Scene
GameObject* obj = scene->CreateGameObject();
scene->DestroyGameObject(*obj);
std::vector<GameObject*> objs = scene->GetGameObjects();
EngineContext& ctx = scene->GetContext();

// GameObject
obj->GetTransform()->SetPosition({x, y, z});
auto comp = obj->AddComponent<ComponentType>();
auto comp = obj->GetComponent<ComponentType>();
obj->Destroy();
obj->Enable(); / obj->Disable();

// Input (via context)
ctx.inputProvider->IsKeyDown(SDL_SCANCODE_SPACE);
ctx.inputProvider->IsKeyJustPressed(SDL_SCANCODE_ESCAPE);
ctx.inputProvider->GetMousePosition();

// Time (via context)
float dt = ctx.timeProvider->GetDeltaTime();
float elapsed = ctx.timeProvider->GetElapsedTime();
float fps = ctx.timeProvider->GetFPS();

// Assets (via context)
ctx.assetProvider->LoadTexture("id", "path/to/texture.png");
SDL_Texture* tex = ctx.assetProvider->GetTexture("id");

// Events
auto id = ctx.events->Subscribe<MyEvent>([](const MyEvent& e) { });
ctx.events->Publish(MyEvent{data});
ctx.events->Unsubscribe<MyEvent>(id);
```

---

## Architecture Advantages

| Advantage | Description |
|-----------|-------------|
| **Code Reusability** | Engine compiled once, linked by multiple games |
| **Parallel Development** | Teams can work on different games simultaneously |
| **Faster Compilation** | Game changes don't require engine recompilation |
| **Clear Separation** | Each game has isolated assets and code |
| **Easy Extension** | Adding games is a single CMake function call |
| **Testability** | Interface-based design enables mocking |
| **Decoupled Systems** | Event bus enables loose coupling |

---

## Notes

- **Windows SDL2**: Requires `#undef main` after `#include <SDL.h>`
- **Assets Path**: Assets are copied to `bin/<GameName>/assets/` during build
- **Logging**: Logs are stored in `logs/` directory with automatic rotation
- **Debug Mode**: Press F3 to toggle debug rendering layer
