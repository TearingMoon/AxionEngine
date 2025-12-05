#include "PhysicsManager.hpp"

PhysicsManager::PhysicsManager(EngineContext &context) : ContextAware(context)
{
    INFO("PhysicsManager initialized.");
}

PhysicsManager::~PhysicsManager()
{
    INFO("PhysicsManager destroyed.");
}

void PhysicsManager::Update(float dt)
{

    accumulatedTime_ += dt;

    if (accumulatedTime_ >= fixedDeltaTime_)
    {
        FixedUpdate(fixedDeltaTime_);
        accumulatedTime_ -= fixedDeltaTime_;
    }
}

void PhysicsManager::FixedUpdate(float dt)
{
    // Get all game objects with colliders from the current scene
    Scene *currentScene = ctx_.scene->GetCurrentScene();
    if (!currentScene)
        return;

    // Get all game objects with colliders
    std::vector<GameObject *> GameObjectsWithCollider = filter_ptr_vector(
        currentScene->GetGameObjects(),
        [](GameObject &obj)
        {
            return obj.HasCollider() && obj.IsEnabled() && !obj.IsDestroyed();
        });

    if (GameObjectsWithCollider.empty())
        return;

    ProcessCollisions(GameObjectsWithCollider);

    // Emit physics update event
    currentScene->EmitFixedUpdateEvent();
}

void PhysicsManager::ProcessCollisions(std::vector<GameObject *> &gameObjectsWithCollider)
{

    std::vector<ColliderEntry> colliders;
    colliders.reserve(gameObjectsWithCollider.size());

    

    // First divide objects into Triggers and Physics

    // Then process Physics Collisions
}

void PhysicsManager::CheckIfNear()
{
}
