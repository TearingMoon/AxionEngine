#include "GameObject.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"

GameObject::GameObject(Scene &parentScene) : parentScene_(parentScene)
{
    // Add base components here
    transform_ = AddComponent<TransformComponent>();
}

void GameObject::Tick()
{
    if (!isEnabled_ || isDestroyed_)
        return;

    if (isFirsUpdate_)
    {
        for (auto &component : components_)
        {
            if (auto updatable = dynamic_cast<IUpdateable *>(component.get()))
            {
                updatable->Start(ctx_());
            }
        }
        isFirsUpdate_ = false;
    }
    else
    {
        for (auto &component : components_)
        {
            if (auto updatable = dynamic_cast<IUpdateable *>(component.get()))
            {
                updatable->Update(ctx_());
            }
        }
    }
}

void GameObject::FixedUpdate()
{
    for (auto &component : components_)
    {
        if (auto updatable = dynamic_cast<IUpdateable *>(component.get()))
        {
            updatable->FixedUpdate(ctx_());
        }
    }
}

void GameObject::Mounted()
{
    isDestroyed_ = false;
    for (auto &component : components_)
    {
        component->OnMounted(ctx_());
    }
}

void GameObject::Destroy()
{
    if (isDestroyed_)
        return; // Already destroyed, avoid double destruction
        
    isDestroyed_ = true;
    for (auto &component : components_)
    {
        component->OnDestroy(ctx_());
    }
}

void GameObject::Enable()
{
    if (isEnabled_)
        return;
    isEnabled_ = true;
    for (auto &component : components_)
    {
        component->OnEnabled(ctx_());
    }
}

void GameObject::Disable()
{
    if (!isEnabled_)
        return;
    isEnabled_ = false;
    for (auto &component : components_)
    {
        component->OnDisabled(ctx_());
    }
}

void GameObject::OnCollisionEnter(GameObject &other)
{
    if (isDestroyed_ || other.IsDestroyed())
        return;
        
    for (auto &component : components_)
    {
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

EngineContext &GameObject::ctx_()
{
    return parentScene_.GetContext();
}
