#include "Scene.hpp"

namespace Axion
{

void Scene::Tick()
{
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
    GameObject *rawPtr = newObject.get();
    spawnQueue_.emplace_back(std::move(newObject));
    return rawPtr;
}

void Scene::DestroyGameObject(GameObject &object)
{
    // Check if already in destroy queue to avoid duplicates
    for (auto *obj : destroyQueue_)
    {
        if (obj == &object)
            return; // Already queued for destruction
    }
    
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
    // Process each object only once, even if duplicates exist
    std::unordered_set<GameObject *> processedObjects;
    
    for (auto *obj : destroyQueue_)
    {
        if (!obj)
            continue;
            
        // Skip if already processed in this batch
        if (processedObjects.find(obj) != processedObjects.end())
            continue;
            
        processedObjects.insert(obj);
        
        // Verify object still exists in objects_ before calling OnDestroy
        bool exists = false;
        for (const auto &ownedObj : objects_)
        {
            if (ownedObj.get() == obj)
            {
                exists = true;
                break;
            }
        }
        
        if (exists)
        {
            // Call OnDestroy to execute component cleanup
            obj->OnDestroy();
            
            // Remove from objects list
            auto it = std::remove_if(objects_.begin(), objects_.end(),
                                     [obj](const std::unique_ptr<GameObject> &ownedObj)
                                     { return ownedObj.get() == obj; });
            objects_.erase(it, objects_.end());
        }
    }
    destroyQueue_.clear();
}

void Scene::ProcessSpawnQueue()
{
    for (auto& obj : spawnQueue_)
    {
        if (obj)
        {
            obj->Mounted();
            objects_.emplace_back(std::move(obj));
        }
    }
    spawnQueue_.clear();
}

} // namespace Axion
