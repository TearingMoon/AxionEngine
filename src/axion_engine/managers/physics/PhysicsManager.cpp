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
    auto *objA = colliderA.GetGameObject();
    auto *objB = colliderB.GetGameObject();

    if (!objA || !objB)
        return false;

    auto *transformA = objA->GetTransform();
    auto *transformB = objB->GetTransform();

    if (!transformA || !transformB)
        return false;

    // TODO: For now, only SphereColliderComponent vs SphereColliderComponent
    auto *sphereA = dynamic_cast<SphereColliderComponent *>(&colliderA);
    auto *sphereB = dynamic_cast<SphereColliderComponent *>(&colliderB);

    if (sphereA && sphereB)
    {
        return CheckSphereSphereCollision(*sphereA, *sphereB, *transformA, *transformB);
    }

    return false;
}

bool PhysicsManager::CheckSphereSphereCollision(const SphereColliderComponent &sphereA, const SphereColliderComponent &sphereB, const TransformComponent &transformA, const TransformComponent &transformB)
{
    glm::vec3 posA = transformA.GetWorldPosition();
    glm::vec3 posB = transformB.GetWorldPosition();

    float radiusA = sphereA.GetRadius();
    float radiusB = sphereB.GetRadius();

    glm::vec3 scaleA = transformA.GetScale();
    glm::vec3 scaleB = transformB.GetScale();
    radiusA *= glm::max(scaleA.x, glm::max(scaleA.y, scaleA.z));
    radiusB *= glm::max(scaleB.x, glm::max(scaleB.y, scaleB.z));

    float distance = glm::distance(posA, posB);
    float minDistance = radiusA + radiusB;

    return distance <= minDistance;
}

void PhysicsManager::ResolveCollision(GameObject &objA, GameObject &objB, ColliderComponent &colliderA, ColliderComponent &colliderB, bool isNewCollision)
{
    if (isNewCollision)
    {
        if (colliderA.IsTrigger() || colliderB.IsTrigger())
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
    else
    {
        return;
    }

    if (colliderA.IsTrigger() || colliderB.IsTrigger())
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
