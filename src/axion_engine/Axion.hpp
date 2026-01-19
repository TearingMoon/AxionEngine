#pragma once

/**
 * @file Axion.hpp
 * @brief Main umbrella header for the Axion Engine.
 *
 * Include this single header to access the complete engine API.
 *
 * @code
 * #include <axion_engine/Axion.hpp>
 * using namespace Axion;
 * @endcode
 */

// Core
#include "axion_engine/core/Engine.hpp"
#include "axion_engine/core/EngineConfig.hpp"
#include "axion_engine/core/EngineContext.hpp"

#include "axion_engine/core/events/Event.hpp"
#include "axion_engine/core/events/EventBus.hpp"

// Platform
#include "axion_engine/platform/window/Window.hpp"
#include "axion_engine/platform/window/WindowConfig.hpp"
#include "axion_engine/platform/sdl_manager/SDLManager.hpp"

// Managers
#include "axion_engine/managers/logger/Logger.hpp"
#include "axion_engine/managers/time/TimeManager.hpp"
#include "axion_engine/managers/input/InputManager.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"
#include "axion_engine/managers/render/RenderManager.hpp"
#include "axion_engine/managers/physics/PhysicsManager.hpp"
#include "axion_engine/managers/assets/AssetsManager.hpp"
#include "axion_engine/managers/analysis/Analyzer.hpp"

// Runtime Classes
#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"

// Interfaces
#include "axion_engine/runtime/interfaces/IUpdateable.hpp"
#include "axion_engine/runtime/interfaces/IRenderable.hpp"
#include "axion_engine/runtime/interfaces/ICollisionListener.hpp"

// Components
#include "axion_engine/runtime/components/Component.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"
#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"
#include "axion_engine/runtime/components/rigid_body/RigidBody.hpp"

// Colliders
#include "axion_engine/runtime/components/collider/ColliderComponent.hpp"
#include "axion_engine/runtime/components/collider/AABBColliderComponent.hpp"
#include "axion_engine/runtime/components/collider/CircleColliderComponent.hpp"
#include "axion_engine/runtime/components/collider/OBBColliderComponent.hpp"

// Renderers
#include "axion_engine/runtime/components/renderer/RenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/SpriteRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/SquareRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/CircleRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/TextRenderComponent.hpp"

// Utilities
#include "axion_utilities/path/PathFinder.hpp"
#include "axion_utilities/vector_filter/PtrVectorFilter.hpp"
