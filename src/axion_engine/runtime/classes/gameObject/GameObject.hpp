#pragma once
#include <vector>
#include <memory>

#include "axion_engine/structure/ContextAware.hpp"
#include "axion_engine/core/EngineContext.hpp"
#include "axion_engine/runtime/interfaces/IUpdateable.hpp"
#include "axion_engine/runtime/interfaces/ICollisionListener.hpp"
#include "axion_engine/runtime/components/Component.hpp"
#include "axion_engine/runtime/components/transform/TransformComponent.hpp"
#include "axion_engine/runtime/components/collider/ColliderComponent.hpp"

namespace Axion
{

class Scene;

/**
 * @brief Base entity class that exists within a Scene.
 * 
 * GameObjects are containers for Components that define behavior and appearance.
 * Every GameObject automatically has a TransformComponent for spatial positioning.
 * 
 * @par Component System:
 * @code
 * auto go = scene->CreateGameObject();
 * go->GetTransform()->SetPosition({100, 200, 0});
 * auto sprite = go->AddComponent<SpriteRenderComponent>();
 * sprite->SetTexture(myTexture);
 * @endcode
 */
class GameObject
{
public:
    GameObject(Scene& parentScene);
    ~GameObject() = default;

    /** @brief Updates all updateable components. */
    void Tick();
    
    /** @brief Runs fixed-timestep physics updates on components. */
    void FixedUpdate();

    /** @brief Called when the object is added to the scene. */
    void Mounted();
    
    /** @brief Requests destruction of this GameObject. */
    void Destroy();
    
    /** @brief Called by Scene when actually destroying the object. */
    void OnDestroy();

    /** @brief Enables the GameObject and its components. */
    void Enable();
    
    /** @brief Disables the GameObject and its components. */
    void Disable();

    /** @brief Called when a physics collision begins with another object. */
    void OnCollisionEnter(GameObject& other);
    
    /** @brief Called when a physics collision ends with another object. */
    void OnCollisionExit(GameObject& other);

    /** @brief Called when entering a trigger volume. */
    void OnTriggerEnter(GameObject& other);
    
    /** @brief Called when exiting a trigger volume. */
    void OnTriggerExit(GameObject& other);

    /**
     * @brief Adds a component of the specified type.
     * @tparam T Component type (must derive from Component)
     * @tparam Args Constructor argument types
     * @param args Arguments forwarded to component constructor
     * @return Pointer to the newly created component
     */
    template <typename T, typename... Args>
    T* AddComponent(Args&&... args);

    /**
     * @brief Finds the first component of the specified type.
     * @tparam T Component type to search for
     * @return Pointer to the component, or nullptr if not found
     */
    template <typename T>
    T* GetComponent() const;

    /**
     * @brief Finds all components of the specified type.
     * @tparam T Component type to search for
     * @return Vector of pointers to matching components
     */
    template <typename T>
    std::vector<T*> GetComponents() const;

    /** @brief Returns all components attached to this GameObject. */
    const std::vector<std::unique_ptr<Component>>& GetAllComponents() const { return components_; }
    
    /** @brief Marks this object for destruction. */
    void MarkAsDestroyed() { isDestroyed_ = true; }

    /** @brief Returns whether this object is enabled. */
    bool IsEnabled() { return isEnabled_; }
    
    /** @brief Returns whether this object has been marked for destruction. */
    bool IsDestroyed() { return isDestroyed_; }
    
    /** @brief Returns the unique ID of this GameObject. */
    size_t GetId() const { return id_; }

    /** @brief Returns whether this object has any collider components. */
    bool HasCollider() const;

    /** @brief Returns the transform component, or nullptr if destroyed. */
    TransformComponent* GetTransform() const 
    { 
        if (isDestroyed_) return nullptr;
        return transform_; 
    }

private:
    Scene& parentScene_;
    GameObject* parent_ = nullptr;
    TransformComponent* transform_;
    
    size_t id_;
    static size_t nextId_;

    EngineContext& ctx_();

    bool isEnabled_ = true;
    bool isDestroyed_ = false;
    bool isFirstUpdate_ = true;

    std::vector<std::unique_ptr<Component>> components_;
};

template <typename T, typename... Args>
inline T* GameObject::AddComponent(Args&&... args)
{
    static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

    auto comp = std::make_unique<T>(std::forward<Args>(args)...);
    comp->Bind(this);

    T* rawPtr = comp.get();
    components_.emplace_back(std::move(comp));
    return rawPtr;
}

template <typename T>
inline T* GameObject::GetComponent() const
{
    for (const auto& comp : components_)
    {
        if (auto casted = dynamic_cast<T*>(comp.get()))
            return casted;
    }
    return nullptr;
}

template <typename T>
inline std::vector<T*> GameObject::GetComponents() const
{
    std::vector<T*> result;
    for (const auto& comp : components_)
    {
        if (auto casted = dynamic_cast<T*>(comp.get()))
            result.push_back(casted);
    }
    return result;
}

} // namespace Axion
