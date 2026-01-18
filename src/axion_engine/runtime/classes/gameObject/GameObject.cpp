#include "GameObject.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"

// Initialize static ID counter
size_t GameObject::nextId_ = 0;

GameObject::GameObject(Scene &parentScene) : parentScene_(parentScene), id_(nextId_++)
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
            if (isDestroyed_) // Check if destroyed during Start()
                return;
            if (!component)
                continue;
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
            if (isDestroyed_) // Check if destroyed during Update()
                return;
            if (!component)
                continue;
            if (auto updatable = dynamic_cast<IUpdateable *>(component.get()))
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
        
    for (auto &component : components_)
    {
        if (isDestroyed_) // Check if destroyed during FixedUpdate()
            return;
        if (!component)
            continue;
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
        
    // Register this object for destruction in the scene
    // This will mark it as destroyed and add it to the destroy queue
    parentScene_.DestroyGameObject(*this);
}

void GameObject::OnDestroy()
{
    // This is called by Scene::ProcessDestroyQueue
    // Execute OnDestroy callbacks for all components
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

EngineContext &GameObject::ctx_()
{
    return parentScene_.GetContext();
}
