#include "Scene.hpp"

void Scene::Tick()
{
    // TODO: Implement Tick logic
    OnSceneUpdate();
    // Update all game objects
    for (auto &object : objects_)
    {
        object->Tick();
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
        object->Mounted();
    }
}

void Scene::EmitEnabledEvent()
{
    for (auto &object : objects_)
    {
        object->Enable();
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
        object->Disable();
    }
}

void Scene::EmitFixedUpdateEvent()
{
    for (auto &object : objects_)
    {
        object->FixedUpdate();
    }
}

void Scene::ProcessDestroyQueue()
{
    for (auto *obj : destroyQueue_)
    {
        obj->Destroy();
        auto it = std::remove_if(objects_.begin(), objects_.end(),
                                 [obj](const std::unique_ptr<GameObject> &ownedObj)
                                 { return ownedObj.get() == obj; });
        objects_.erase(it, objects_.end());
    }
    destroyQueue_.clear();
}
