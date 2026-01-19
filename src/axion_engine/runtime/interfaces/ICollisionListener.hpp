#pragma once

namespace Axion
{

class GameObject;

/**
 * @brief Interface for components that respond to collision events.
 * 
 * Components implementing ICollisionListener receive callbacks when
 * their GameObject collides with or triggers other GameObjects.
 */
class ICollisionListener
{
public:
    virtual ~ICollisionListener() = default;

    /** @brief Called when a physics collision begins. */
    virtual void OnCollisionEnter(GameObject& other) {}
    
    /** @brief Called when a physics collision ends. */
    virtual void OnCollisionExit(GameObject& other) {}

    /** @brief Called when entering a trigger volume. */
    virtual void OnTriggerEnter(GameObject& other) {}
    
    /** @brief Called when exiting a trigger volume. */
    virtual void OnTriggerExit(GameObject& other) {}
};

} // namespace Axion