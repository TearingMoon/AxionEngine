#include "GameObject.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"

namespace Axion
{

size_t GameObject::nextId_ = 0;

GameObject::GameObject(Scene& parentScene) : parentScene_(parentScene), id_(nextId_++)
{
    transform_ = AddComponent<TransformComponent>();
}

void GameObject::Tick()
{
    if (!isEnabled_ || isDestroyed_)
        return;

    if (isFirstUpdate_)
    {
        for (size_t index = 0; index < components_.size(); ++index)
        {
            if (isDestroyed_)
                return;
            Component* component = components_[index].get();
            if (!component)
                continue;
            if (auto updatable = dynamic_cast<IUpdateable*>(component))
            {
                updatable->Start(ctx_());
            }
        }
        isFirstUpdate_ = false;
        updatesStarted_ = true;
    }
    else
    {
        for (size_t index = 0; index < components_.size(); ++index)
        {
            if (isDestroyed_)
                return;
            Component* component = components_[index].get();
            if (!component)
                continue;
            if (auto updatable = dynamic_cast<IUpdateable*>(component))
            {
                updatable->Update(ctx_());
            }
        }
    }
}

void GameObject::FixedUpdate()
{
    if (!isEnabled_ || isDestroyed_)
        return;
        
    for (size_t index = 0; index < components_.size(); ++index)
    {
        if (isDestroyed_) // Check if destroyed during FixedUpdate()
            return;
        Component* component = components_[index].get();
        if (!component)
            continue;
        if (auto updatable = dynamic_cast<IUpdateable *>(component))
        {
            updatable->FixedUpdate(ctx_());
        }
    }
}

void GameObject::LateUpdate()
{
    if (!isEnabled_ || isDestroyed_)
        return;

    for (size_t index = 0; index < components_.size(); ++index)
    {
        if (isDestroyed_)
            return;

        Component* component = components_[index].get();
        if (!component)
            continue;

        if (auto updatable = dynamic_cast<IUpdateable*>(component))
        {
            updatable->LateUpdate(ctx_());
        }
    }
}

void GameObject::Mounted()
{
    if (isMounted_)
        return;

    isDestroyed_ = false;
    destroyCallbacksInvoked_ = false;
    isMounted_ = true;

    for (size_t index = 0; index < components_.size(); ++index)
    {
        Component* component = components_[index].get();
        if (!component || component->mounted_)
            continue;
        component->mounted_ = true;
        component->OnMounted(ctx_());
    }

    if (isEnabled_)
    {
        for (size_t index = 0; index < components_.size(); ++index)
        {
            Component* component = components_[index].get();
            if (!component || component->enabled_)
                continue;
            component->enabled_ = true;
            component->OnEnabled(ctx_());
        }
    }
}

void GameObject::Destroy()
{
    if (isDestroyed_)
        return; // Already destroyed, avoid double destruction
        
    // Register this object for destruction in the scene
    // This will mark it as destroyed and add it to the destroy queue
    parentScene_.DestroyGameObject(*this);
}

void GameObject::OnDestroy()
{
    if (destroyCallbacksInvoked_)
        return;

    destroyCallbacksInvoked_ = true;

    // This is called by Scene::ProcessDestroyQueue
    // Execute OnDestroy callbacks for all components
    for (size_t index = 0; index < components_.size(); ++index)
    {
        Component* component = components_[index].get();
        if (!component)
            continue;
        component->OnDestroy(ctx_());
    }
}

void GameObject::Enable()
{
    if (isEnabled_ || isDestroyed_)
        return;

    isEnabled_ = true;
    if (!isMounted_)
        return;

    for (size_t index = 0; index < components_.size(); ++index)
    {
        Component* component = components_[index].get();
        if (!component || component->enabled_)
            continue;
        component->enabled_ = true;
        component->OnEnabled(ctx_());
    }
}

void GameObject::Disable()
{
    if (!isEnabled_)
        return;
    isEnabled_ = false;

    if (!isMounted_)
        return;

    for (size_t index = 0; index < components_.size(); ++index)
    {
        Component* component = components_[index].get();
        if (!component || !component->enabled_)
            continue;
        component->enabled_ = false;
        component->OnDisabled(ctx_());
    }
}

void GameObject::OnCollisionEnter(GameObject &other)
{
    if (isDestroyed_ || other.IsDestroyed())
        return;
        
    for (auto &component : components_)
    {
        if (isDestroyed_ || other.IsDestroyed()) // Check again after each callback
            return;
        if (!component)
            continue;
        if (auto collisionListener = dynamic_cast<ICollisionListener *>(component.get()))
        {
            collisionListener->OnCollisionEnter(other);
        }
    }
}

void GameObject::OnCollisionExit(GameObject &other)
{
    if (isDestroyed_ || other.IsDestroyed())
        return;
        
    for (auto &component : components_)
    {
        if (isDestroyed_ || other.IsDestroyed()) // Check again after each callback
            return;
        if (!component)
            continue;
        if (auto collisionListener = dynamic_cast<ICollisionListener *>(component.get()))
        {
            collisionListener->OnCollisionExit(other);
        }
    }
}

void GameObject::OnTriggerEnter(GameObject &other)
{
    if (isDestroyed_ || other.IsDestroyed())
        return;
        
    for (auto &component : components_)
    {
        if (isDestroyed_ || other.IsDestroyed()) // Check again after each callback
            return;
        if (!component)
            continue;
        if (auto collisionListener = dynamic_cast<ICollisionListener *>(component.get()))
        {
            collisionListener->OnTriggerEnter(other);
        }
    }
}

void GameObject::OnTriggerExit(GameObject &other)
{
    if (isDestroyed_ || other.IsDestroyed())
        return;
        
    for (auto &component : components_)
    {
        if (isDestroyed_ || other.IsDestroyed()) // Check again after each callback
            return;
        if (!component)
            continue;
        if (auto collisionListener = dynamic_cast<ICollisionListener *>(component.get()))
        {
            collisionListener->OnTriggerExit(other);
        }
    }
}

bool GameObject::HasCollider() const
{ // TODO: Check if this is the most efficient way, maybe store a bool that is updated when adding/removing components?
    for (const auto &comp : components_)
    {
        if (dynamic_cast<ColliderComponent *>(comp.get()))
        {
            return true;
        }
    }
    return false;
}

EngineContext& GameObject::ctx_()
{
    return parentScene_.GetContext();
}

} // namespace Axion
