#include "Scene.hpp"

void Scene::Tick()
{
    // TODO: Implement Tick logic
    OnSceneUpdate();
    // Update all game objects
    for (auto &object : objects_)
    {
        // Skip destroyed objects
        if (object && !object->IsDestroyed())
        {
            object->Tick();
        }
    }
}

void Scene::Draw()
{
    // TODO: Implement Draw logic
}

GameObject *Scene::CreateGameObject()
{
    auto newObject = std::make_unique<GameObject>(*this);
    objects_.emplace_back(std::move(newObject));
    return objects_.back().get();
}

void Scene::DestroyGameObject(GameObject &object)
{
    object.Disable();
    object.MarkAsDestroyed();
    destroyQueue_.push_back(&object);
}

void Scene::EmitMountedEvent()
{
    for (auto &object : objects_)
    {
        if (object && !object->IsDestroyed())
        {
            object->Mounted();
        }
    }
}

void Scene::EmitEnabledEvent()
{
    for (auto &object : objects_)
    {
        if (object && !object->IsDestroyed() && object->IsEnabled())
        {
            object->Enable();
        }
    }
}

void Scene::EmitDestroyedEvent()
{
    for (auto &object : objects_)
    {
        object->Destroy();
    }
}

void Scene::EmitDisabledEvent()
{
    for (auto &object : objects_)
    {
        if (object && !object->IsDestroyed())
        {
            object->Disable();
        }
    }
}

void Scene::EmitFixedUpdateEvent()
{
    for (auto &object : objects_)
    {
        // Skip destroyed objects
        if (object && !object->IsDestroyed())
        {
            object->FixedUpdate();
        }
    }
}

void Scene::ProcessDestroyQueue()
{
    for (auto *obj : destroyQueue_)
    {
        // Only call Destroy if not already destroyed
        if (obj && !obj->IsDestroyed())
        {
            obj->Destroy();
        }
        
        // Remove from objects list
        auto it = std::remove_if(objects_.begin(), objects_.end(),
                                 [obj](const std::unique_ptr<GameObject> &ownedObj)
                                 { return ownedObj.get() == obj; });
        objects_.erase(it, objects_.end());
    }
    destroyQueue_.clear();
}
