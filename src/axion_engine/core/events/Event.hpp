#pragma once

#include <string>
#include <typeinfo>

namespace Axion
{

/**
 * @brief Base class for all events in the engine.
 * 
 * Events are used for decoupled communication between systems.
 * Derive from this class to create custom events.
 * 
 * @example
 * struct PlayerDiedEvent : public Event {
 *     int playerId;
 *     std::string cause;
 * };
 */
struct Event
{
    virtual ~Event() = default;
    
    /** @brief Returns true if the event has been consumed and should not propagate further. */
    bool IsHandled() const { return handled_; }
    
    /** @brief Marks the event as handled, preventing further propagation. */
    void MarkHandled() { handled_ = true; }
    
private:
    bool handled_ = false;
};

/**
 * @brief Event fired when a scene change is requested.
 */
struct SceneChangeEvent : public Event
{
    std::string sceneName;
    
    explicit SceneChangeEvent(const std::string& name) : sceneName(name) {}
};

/**
 * @brief Event fired when a GameObject is spawned.
 */
struct GameObjectSpawnedEvent : public Event
{
    class GameObject* gameObject;
    
    explicit GameObjectSpawnedEvent(class GameObject* go) : gameObject(go) {}
};

/**
 * @brief Event fired when a GameObject is destroyed.
 */
struct GameObjectDestroyedEvent : public Event
{
    class GameObject* gameObject;
    
    explicit GameObjectDestroyedEvent(class GameObject* go) : gameObject(go) {}
};

/**
 * @brief Event fired when collision occurs between two objects.
 */
struct CollisionEvent : public Event
{
    class GameObject* objectA;
    class GameObject* objectB;
    
    CollisionEvent(class GameObject* a, class GameObject* b)
        : objectA(a), objectB(b) {}
};

} // namespace Axion
