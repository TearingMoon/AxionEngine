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

    ProcessCollisions(GameObjectsWithCollider);

    // Emit physics update event
    currentScene->EmitFixedUpdateEvent();
}

void PhysicsManager::ProcessCollisions(std::vector<GameObject *> &gameObjectsWithCollider)
{
    if (gameObjectsWithCollider.size() < 2)
        return;

    currentCollisions_.clear();

    std::vector<std::pair<GameObject *, ColliderComponent *>> colliderPairs;

    for (auto *obj : gameObjectsWithCollider)
    {
        auto colliders = obj->GetComponents<ColliderComponent>();
        for (auto *collider : colliders)
        {
            colliderPairs.push_back({obj, collider});
        }
    }

    for (size_t i = 0; i < colliderPairs.size() - 1; ++i)
    {
        for (size_t j = i + 1; j < colliderPairs.size(); ++j)
        {
            auto &[objA, colliderA] = colliderPairs[i];
            auto &[objB, colliderB] = colliderPairs[j];

            if (objA == objB)
                continue;

            auto pair = colliderA < colliderB
                            ? std::make_pair(colliderA, colliderB)
                            : std::make_pair(colliderB, colliderA);

            if (CheckCollision(*colliderA, *colliderB))
            {
                currentCollisions_.insert(pair);
                bool isNewCollision = previousCollisions_.find(pair) == previousCollisions_.end();
                ResolveCollision(*objA, *objB, *colliderA, *colliderB, isNewCollision);
            }
        }
    }

    DetectCollisionEvents();
    previousCollisions_ = currentCollisions_;
}

bool PhysicsManager::CheckCollision(ColliderComponent &colliderA, ColliderComponent &colliderB)
{
    Manifold manifold;
    return colliderA.Intersects(colliderB, manifold);
}

void PhysicsManager::ResolveCollision(GameObject &objA, GameObject &objB, ColliderComponent &colliderA, ColliderComponent &colliderB, bool isNewCollision)
{
    bool isTrigger = colliderA.IsTrigger() || colliderB.IsTrigger();

    if (isNewCollision)
    {
        if (isTrigger)
        {
            objA.OnTriggerEnter(objB);
            objB.OnTriggerEnter(objA);
        }
        else
        {
            objA.OnCollisionEnter(objB);
            objB.OnCollisionEnter(objA);
        }
    }

    // Skip physics resolution for triggers
    if (isTrigger)
        return;

    // TODO: Add physics resolution here
}

void PhysicsManager::DetectCollisionEvents()
{
    for (const auto &pair : previousCollisions_)
    {
        if (currentCollisions_.find(pair) == currentCollisions_.end())
        {
            auto *colliderA = pair.first;
            auto *colliderB = pair.second;
            auto *objA = colliderA->GetGameObject();
            auto *objB = colliderB->GetGameObject();

            if (objA && objB)
            {
                if (colliderA->IsTrigger() || colliderB->IsTrigger())
                {
                    objA->OnTriggerExit(*objB);
                    objB->OnTriggerExit(*objA);
                    continue;
                }
                else
                {
                    objA->OnCollisionExit(*objB);
                    objB->OnCollisionExit(*objA);
                }
            }
        }
    }
}
