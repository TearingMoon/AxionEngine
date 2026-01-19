#pragma once
#include "axion_engine/core/EngineContext.hpp"

namespace Axion
{

/**
 * @brief Interface for components that receive update callbacks.
 * 
 * Components implementing IUpdateable will receive lifecycle callbacks
 * for initialization and per-frame updates.
 */
class IUpdateable
{
public:
    virtual ~IUpdateable() = default;
    
    /** @brief Called once when the component starts (first frame). */
    virtual void Start(EngineContext& context) {}
    
    /** @brief Called every frame for game logic updates. */
    virtual void Update(EngineContext& context) {}

    /** @brief Called at a fixed rate for physics-related updates. */
    virtual void FixedUpdate(EngineContext& context) {}
    
    /** @brief Called after all Update calls have completed. */
    virtual void LateUpdate(EngineContext& context) {}
};

} // namespace Axion