#include "PhysicsManager.hpp"

namespace Axion
{

PhysicsManager::PhysicsManager(EngineContext& context) : ContextAware(context)
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
    
    // Get all game objects with rigid bodies
    std::vector<GameObject *> GameObjectsWithRigidBody = filter_ptr_vector(
        currentScene->GetGameObjects(),
        [](GameObject &obj)
        {
            return obj.GetComponent<RigidBody>() != nullptr && obj.IsEnabled() && !obj.IsDestroyed();
        });

    ProcessForces(GameObjectsWithRigidBody);

    // Emit physics update event
    currentScene->EmitFixedUpdateEvent();
}

void PhysicsManager::ProcessCollisions(std::vector<GameObject *> &gameObjectsWithCollider)
{
    if (gameObjectsWithCollider.size() < 2)
        return;

    currentCollisions_.clear();
    
    // Build map of GameObject ID to GameObject pointer
    std::unordered_map<size_t, GameObject *> idToObject;
    for (auto *obj : gameObjectsWithCollider)
    {
        if (obj && !obj->IsDestroyed())
            idToObject[obj->GetId()] = obj;
    }

    std::unordered_set<std::pair<size_t, size_t>, PairHashSize> newCollisionIds;

    // Store only GameObject IDs, not collider pointers
    std::vector<size_t> objectIds;
    objectIds.reserve(gameObjectsWithCollider.size());
    
    for (auto *obj : gameObjectsWithCollider)
    {
        if (obj && !obj->IsDestroyed())
            objectIds.push_back(obj->GetId());
    }

    // Double loop through object IDs
    for (size_t i = 0; i < objectIds.size(); ++i)
    {
        for (size_t j = i + 1; j < objectIds.size(); ++j)
        {
            size_t idA = objectIds[i];
            size_t idB = objectIds[j];
            
            // Get fresh objects from map
            auto itA = idToObject.find(idA);
            auto itB = idToObject.find(idB);
            
            if (itA == idToObject.end() || itB == idToObject.end())
                continue;
            
            auto *objA = itA->second;
            auto *objB = itB->second;
            
            if (!objA || !objB)
                continue;
            
            if (objA->IsDestroyed() || objB->IsDestroyed())
            {
                idToObject.erase(idA);
                idToObject.erase(idB);
                continue;
            }
            
            // Get fresh colliders NOW
            auto collidersA = objA->GetComponents<ColliderComponent>();
            auto collidersB = objB->GetComponents<ColliderComponent>();
            
            // Test each collider pair
            for (auto *colliderA : collidersA)
            {
                if (!colliderA) continue;
                
                for (auto *colliderB : collidersB)
                {
                    if (!colliderB) continue;
                    
                    auto pair = colliderA < colliderB
                                    ? std::make_pair(colliderA, colliderB)
                                    : std::make_pair(colliderB, colliderA);
                    
                    Manifold manifold;
                    
                    // Double-check objects still valid before Intersects
                    if (objA->IsDestroyed() || objB->IsDestroyed())
                        break;
                    
                    bool intersected = colliderA->Intersects(*colliderB, manifold);
                    
                    if (intersected)
                    {
                        currentCollisions_.insert(pair);
                        
                        auto idPair = idA < idB ? std::make_pair(idA, idB) : std::make_pair(idB, idA);
                        newCollisionIds.insert(idPair);
                        
                        bool isNewCollision = previousCollisionIds_.find(idPair) == previousCollisionIds_.end();
                        
                        ResolveCollision(*objA, *objB, *colliderA, *colliderB, isNewCollision, manifold);
                        
                        // Objects might be destroyed after callback
                        if (objA->IsDestroyed() || objB->IsDestroyed())
                        {
                            idToObject.erase(idA);
                            idToObject.erase(idB);
                            goto next_object_pair;
                        }
                    }
                }
            }
            
            next_object_pair:;
        }
    }

    DetectCollisionEvents(idToObject, newCollisionIds);
    previousCollisionIds_ = std::move(newCollisionIds);
}

void PhysicsManager::ProcessForces(std::vector<GameObject *> &gameObjectsWithRigidBody)
{
    Scene *currentScene = ctx_.scene->GetCurrentScene();
    if (!currentScene)
        return;

    glm::vec3 gravity = currentScene->Gravity;

    for (auto *obj : gameObjectsWithRigidBody)
    {
        auto *rigidBody = obj->GetComponent<RigidBody>();
        if (rigidBody)
        {
            // First integrate forces (including gravity) to update velocity
            rigidBody->IntegrateForces(fixedDeltaTime_, gravity);
            // Then integrate velocity to update position
            rigidBody->IntegrateVelocity(fixedDeltaTime_);
        }
    }
}

void PhysicsManager::ResolveCollision(GameObject &objA, GameObject &objB, ColliderComponent &colliderA, ColliderComponent &colliderB, bool isNewCollision, Manifold &manifold)
{
    bool isTrigger = colliderA.IsTrigger() || colliderB.IsTrigger();

    if (isNewCollision)
    {
        // Check if objects are destroyed before calling callbacks
        if (objA.IsDestroyed() || objB.IsDestroyed())
            return;
            
        if (isTrigger)
        {
            objA.OnTriggerEnter(objB);
            // Check again after first callback as it might destroy objects
            if (objA.IsDestroyed() || objB.IsDestroyed())
            {
                // Remove this collision pair from currentCollisions
                auto pair = &colliderA < &colliderB
                                ? std::make_pair(&colliderA, &colliderB)
                                : std::make_pair(&colliderB, &colliderA);
                currentCollisions_.erase(pair);
                // No need to remove from previousCollisionIds_ - objects won't exist next frame
                return;
            }
            objB.OnTriggerEnter(objA);
            // Check again after second callback
            if (objA.IsDestroyed() || objB.IsDestroyed())
            {
                auto pair = &colliderA < &colliderB
                                ? std::make_pair(&colliderA, &colliderB)
                                : std::make_pair(&colliderB, &colliderA);
                currentCollisions_.erase(pair);
                return;
            }
        }
        else
        {
            objA.OnCollisionEnter(objB);
            // Check again after first callback as it might destroy objects
            if (objA.IsDestroyed() || objB.IsDestroyed())
            {
                auto pair = &colliderA < &colliderB
                                ? std::make_pair(&colliderA, &colliderB)
                                : std::make_pair(&colliderB, &colliderA);
                currentCollisions_.erase(pair);
                return;
            }
            objB.OnCollisionEnter(objA);
            // Check again after second callback
            if (objA.IsDestroyed() || objB.IsDestroyed())
            {
                auto pair = &colliderA < &colliderB
                                ? std::make_pair(&colliderA, &colliderB)
                                : std::make_pair(&colliderB, &colliderA);
                currentCollisions_.erase(pair);
                return;
            }
        }
    }

    // Skip physics resolution for triggers
    if (isTrigger)
        return;

    // TODO: Add physics resolution here
    // Get the RigidBodys of both

    auto rigidA = objA.GetComponent<RigidBody>();
    auto rigidB = objB.GetComponent<RigidBody>();

    if (!rigidA && !rigidB)
        return;

    // Simple collision response: separate objects along the collision normal
    float totalInverseMass = 0.0f;
    totalInverseMass += 1.0f / rigidA->GetMass();
    totalInverseMass += 1.0f / rigidB->GetMass();
    if (totalInverseMass == 0.0f)
        return;

    // Separate objects based on penetration depth
    float percent = 0.8f; // Penetration percentage to correct (0.2 to 1.0)
    float slop = 0.01f;   // Penetration allowance
    float correctionMagnitude = glm::max(manifold.penetration - slop, 0.0f) / totalInverseMass * percent;
    glm::vec2 correction = correctionMagnitude * manifold.normal;

    if (rigidA)
    {
        glm::vec3 posA = objA.GetTransform()->GetWorldPosition();
        float invMassA = 1.0f / rigidA->GetMass();
        posA.x -= correction.x * invMassA;
        posA.y -= correction.y * invMassA;
        objA.GetTransform()->SetPosition(posA);
    }

    if (rigidB)
    {
        glm::vec3 posB = objB.GetTransform()->GetWorldPosition();
        float invMassB = 1.0f / rigidB->GetMass();
        posB.x += correction.x * invMassB;
        posB.y += correction.y * invMassB;
        objB.GetTransform()->SetPosition(posB);
    }

    // Apply impulse for velocity correction
    glm::vec3 relativeVelocity = glm::vec3(0.0f);
    if (rigidA)
        relativeVelocity -= rigidA->GetVelocity();
    if (rigidB)
        relativeVelocity += rigidB->GetVelocity();

    float velocityAlongNormal = glm::dot(glm::vec2(relativeVelocity.x, relativeVelocity.y), manifold.normal);

    // Don't resolve if velocities are separating
    if (velocityAlongNormal > 0)
        return;

    // Calculate restitution (bounciness)
    float restitution = 0.2f; // 0 = inelastic, 1 = perfectly elastic

    // Calculate impulse scalar
    float impulseScalar = -(1.0f + restitution) * velocityAlongNormal;
    impulseScalar /= totalInverseMass;

    // Apply impulse
    glm::vec2 impulse = impulseScalar * manifold.normal;

    if (rigidA)
    {
        glm::vec3 velA = rigidA->GetVelocity();
        velA.x -= impulse.x * (1.0f / rigidA->GetMass());
        velA.y -= impulse.y * (1.0f / rigidA->GetMass());
        rigidA->SetVelocity(velA);
    }

    if (rigidB)
    {
        glm::vec3 velB = rigidB->GetVelocity();
        velB.x += impulse.x * (1.0f / rigidB->GetMass());
        velB.y += impulse.y * (1.0f / rigidB->GetMass());
        rigidB->SetVelocity(velB);
    }
}

void PhysicsManager::DetectCollisionEvents(const std::unordered_map<size_t, GameObject *> &idToObject,
                                           const std::unordered_set<std::pair<size_t, size_t>, PairHashSize> &currentCollisionIds)
{
    // Now previousCollisionIds_ contains IDs (safe) and idToObject maps to current valid objects
    // This completely avoids the dangling pointer problem
    
    for (const auto &idPair : previousCollisionIds_)
    {
        // Check if this collision still exists in current frame using IDs only
        bool stillColliding = currentCollisionIds.find(idPair) != currentCollisionIds.end();
        
        if (!stillColliding)
        {
            // Collision ended - trigger exit events
            auto itA = idToObject.find(idPair.first);
            auto itB = idToObject.find(idPair.second);
            
            // Only trigger if both objects still exist
            if (itA != idToObject.end() && itB != idToObject.end())
            {
                auto *objA = itA->second;
                auto *objB = itB->second;
                
                if (objA && objB && !objA->IsDestroyed() && !objB->IsDestroyed())
                {
                    // Get colliders to check if it's a trigger
                    auto collidersA = objA->GetComponents<ColliderComponent>();
                    auto collidersB = objB->GetComponents<ColliderComponent>();
                    
                    bool isTrigger = false;
                    for (auto *coll : collidersA)
                    {
                        if (coll && coll->IsTrigger())
                        {
                            isTrigger = true;
                            break;
                        }
                    }
                    if (!isTrigger)
                    {
                        for (auto *coll : collidersB)
                        {
                            if (coll && coll->IsTrigger())
                            {
                                isTrigger = true;
                                break;
                            }
                        }
                    }
                    
                    if (isTrigger)
                    {
                        objA->OnTriggerExit(*objB);
                        if (!objA->IsDestroyed() && !objB->IsDestroyed())
                            objB->OnTriggerExit(*objA);
                    }
                    else
                    {
                        objA->OnCollisionExit(*objB);
                        if (!objA->IsDestroyed() && !objB->IsDestroyed())
                            objB->OnCollisionExit(*objA);
                    }
                }
            }
        }
    }
}

} // namespace Axion
