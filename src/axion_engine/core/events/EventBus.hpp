#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>
#include <algorithm>

#include "Event.hpp"

namespace Axion
{

/**
 * @brief Central event bus for publish-subscribe communication.
 * 
 * EventBus provides a decoupled communication mechanism where
 * publishers emit events without knowing who will receive them,
 * and subscribers receive events without knowing who sent them.
 * 
 * @par Usage:
 * @code
 * // Subscribe to an event
 * auto id = eventBus.Subscribe<PlayerDiedEvent>([](const PlayerDiedEvent& e) {
 *     std::cout << "Player " << e.playerId << " died!\n";
 * });
 * 
 * // Publish an event
 * eventBus.Publish(PlayerDiedEvent{1, "fall damage"});
 * 
 * // Unsubscribe when no longer needed
 * eventBus.Unsubscribe<PlayerDiedEvent>(id);
 * @endcode
 * 
 * @note Thread safety: Not thread-safe. Use from main thread only.
 */
class EventBus
{
public:
    using SubscriberId = size_t;
    
    EventBus() = default;
    ~EventBus() = default;
    
    // Non-copyable, non-movable
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    EventBus(EventBus&&) = delete;
    EventBus& operator=(EventBus&&) = delete;
    
    /**
     * @brief Subscribes to events of type T.
     * @tparam T Event type (must derive from Event)
     * @param callback Function to call when event is published
     * @return Subscriber ID for later unsubscription
     */
    template<typename T>
    SubscriberId Subscribe(std::function<void(const T&)> callback)
    {
        static_assert(std::is_base_of_v<Event, T>, "T must derive from Event");
        
        auto typeIdx = std::type_index(typeid(T));
        auto id = nextId_++;
        
        auto wrapper = [callback](const Event& e) {
            callback(static_cast<const T&>(e));
        };
        
        handlers_[typeIdx].push_back({id, wrapper});
        return id;
    }
    
    /**
     * @brief Unsubscribes from events of type T.
     * @tparam T Event type
     * @param id Subscriber ID returned from Subscribe()
     */
    template<typename T>
    void Unsubscribe(SubscriberId id)
    {
        auto typeIdx = std::type_index(typeid(T));
        auto it = handlers_.find(typeIdx);
        if (it == handlers_.end())
            return;
        
        auto& vec = it->second;
        vec.erase(
            std::remove_if(vec.begin(), vec.end(),
                [id](const Handler& h) { return h.id == id; }),
            vec.end()
        );
    }
    
    /**
     * @brief Publishes an event to all subscribers.
     * @tparam T Event type
     * @param event The event to publish
     */
    template<typename T>
    void Publish(T& event)
    {
        static_assert(std::is_base_of_v<Event, T>, "T must derive from Event");
        
        auto typeIdx = std::type_index(typeid(T));
        auto it = handlers_.find(typeIdx);
        if (it == handlers_.end())
            return;
        
        for (const auto& handler : it->second)
        {
            if (event.IsHandled())
                break;
            handler.callback(event);
        }
    }
    
    /**
     * @brief Publishes an event (rvalue version).
     * @tparam T Event type
     * @param event The event to publish
     */
    template<typename T>
    void Publish(T&& event)
    {
        T e = std::move(event);
        Publish(e);
    }
    
    /** @brief Removes all subscribers. */
    void Clear()
    {
        handlers_.clear();
    }
    
private:
    struct Handler
    {
        SubscriberId id;
        std::function<void(const Event&)> callback;
    };
    
    std::unordered_map<std::type_index, std::vector<Handler>> handlers_;
    SubscriberId nextId_ = 1;
};

} // namespace Axion
