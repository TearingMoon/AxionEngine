#pragma once
#include "axion_engine/core/EngineContext.hpp"

namespace Axion
{

class GameObject;

/**
 * @brief Base class for all components that can be attached to GameObjects.
 * 
 * Components add functionality to GameObjects through composition.
 * Each component has lifecycle callbacks for mounting, destruction,
 * and enable/disable state changes.
 */
class Component
{
public:
    virtual ~Component() = default;

    /**
     * @brief Binds this component to a GameObject.
     * @param owner The GameObject that owns this component
     */
    void Bind(GameObject* owner) { owner_ = owner; }
    
    /** @brief Returns the GameObject that owns this component. */
    GameObject* GetOwner() const { return owner_; }

    /** @brief Called when the component is first added to the scene. */
    virtual void OnMounted(EngineContext& context) {}
    
    /** @brief Called when the component is being destroyed. */
    virtual void OnDestroy(EngineContext& context) {}

    /** @brief Called when the owning GameObject is enabled. */
    virtual void OnEnabled(EngineContext& context) {}
    
    /** @brief Called when the owning GameObject is disabled. */
    virtual void OnDisabled(EngineContext& context) {}

private:
    GameObject* owner_ = nullptr;
};

} // namespace Axion