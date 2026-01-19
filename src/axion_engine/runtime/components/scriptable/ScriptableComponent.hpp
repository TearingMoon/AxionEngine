#pragma once

#include "axion_engine/runtime/components/Component.hpp"

#include "axion_engine/runtime/interfaces/IUpdateable.hpp"
#include "axion_engine/runtime/interfaces/ICollisionListener.hpp"

namespace Axion
{

/**
 * @brief Base class for custom game logic components.
 * 
 * ScriptableComponent combines Component, IUpdateable, and ICollisionListener
 * interfaces, providing a convenient base for game-specific behaviors.
 * 
 * @par Usage:
 * @code
 * class PlayerController : public Axion::ScriptableComponent {
 * public:
 *     void Start(Axion::EngineContext& ctx) override {
 *         // Initialize
 *     }
 *     
 *     void Update(Axion::EngineContext& ctx) override {
 *         // Per-frame logic
 *     }
 *     
 *     void OnCollisionEnter(Axion::GameObject& other) override {
 *         // Handle collision
 *     }
 * };
 * @endcode
 */
class ScriptableComponent : public Component, public IUpdateable, public ICollisionListener
{
public:
    virtual ~ScriptableComponent() = default;
};

} // namespace Axion