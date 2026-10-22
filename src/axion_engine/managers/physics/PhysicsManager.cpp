#include "PhysicsManager.hpp"

namespace Axion
{

    PhysicsManager::PhysicsManager(EngineContext &context) : ContextAware(context)
    {
        AXION_INFO("PhysicsManager initialized.");
    }

    PhysicsManager::~PhysicsManager()
    {
        AXION_INFO("PhysicsManager destroyed.");
    }

    void PhysicsManager::Update(float dt)
    {
        accumulatedTime_ += dt;

        constexpr int maxSubSteps = 8;
        int subSteps = 0;

        while (accumulatedTime_ >= fixedDeltaTime_ &&
               subSteps < maxSubSteps)
        {
            FixedUpdate(fixedDeltaTime_);
            accumulatedTime_ -= fixedDeltaTime_;
            ++subSteps;
        }
    }

    void PhysicsManager::FixedUpdate(float dt)
    {
        Scene *scene = ctx_.sceneProvider->GetCurrentScene();
        if (!scene)
            return;

        colliders_.clear();
        rigidBodies_.clear();

        // Collect active physics objects in a single pass
        for (GameObject *obj : scene->GetGameObjects())
        {
            if (!obj || !obj->IsEnabled() || obj->IsDestroyed())
                continue;

            if (obj->HasCollider())
                colliders_.push_back(obj);

            if (obj->GetComponent<RigidBody>() && !obj->GetComponent<RigidBody>()->IsStatic())
                rigidBodies_.push_back(obj);
        }

        ProcessCollisions(colliders_);
        ProcessForces(rigidBodies_, dt);

        scene->EmitFixedUpdateEvent();
    }

    void PhysicsManager::ProcessCollisions(
        const std::vector<GameObject *> &objects)
    {
        currentCollisions_.clear();

        // Map object IDs to their corresponding GameObjects for event dispatch
        std::unordered_map<size_t, GameObject *> idToObject;
        idToObject.reserve(objects.size());

        for (GameObject *obj : objects)
        {
            if (!obj || obj->IsDestroyed())
                continue;

            idToObject.emplace(obj->GetId(), obj);
        }

        // Store collision pairs detected during the current physics step
        std::unordered_set<
            std::pair<size_t, size_t>,
            PairHashSize>
            newCollisionIds;

        newCollisionIds.reserve(previousCollisionIds_.size());

        // Test each unique pair of GameObjects
        for (size_t i = 0; i < objects.size(); ++i)
        {
            GameObject *objA = objects[i];

            if (!objA || objA->IsDestroyed() || !objA->IsEnabled())
                continue;

            const size_t idA = objA->GetId();

            for (size_t j = i + 1; j < objects.size(); ++j)
            {
                // Callbacks may destroy or disable the first object
                if (objA->IsDestroyed() || !objA->IsEnabled())
                    break;

                GameObject *objB = objects[j];

                if (!objB || objB->IsDestroyed() || !objB->IsEnabled())
                    continue;

                const size_t idB = objB->GetId();

                // Retrieve components only for the current object pair
                auto collidersA = objA->GetComponents<ColliderComponent>();
                auto collidersB = objB->GetComponents<ColliderComponent>();

                bool stopPair = false;

                for (ColliderComponent *colliderA : collidersA)
                {
                    if (objA->IsDestroyed() || objB->IsDestroyed() ||
                        !objA->IsEnabled() || !objB->IsEnabled())
                    {
                        stopPair = true;
                        break;
                    }

                    if (!colliderA)
                        continue;

                    for (ColliderComponent *colliderB : collidersB)
                    {
                        if (objA->IsDestroyed() || objB->IsDestroyed() ||
                            !objA->IsEnabled() || !objB->IsEnabled())
                        {
                            stopPair = true;
                            break;
                        }

                        if (!colliderB)
                            continue;

                        Manifold manifold;

                        // Perform narrow-phase collision detection
                        if (!colliderA->Intersects(*colliderB, manifold))
                            continue;

                        // Normalize collider pair ordering
                        const auto colliderPair = std::minmax(
                            colliderA,
                            colliderB,
                            std::less<ColliderComponent *>{});

                        currentCollisions_.insert(colliderPair);

                        // Normalize GameObject ID ordering
                        const auto idPair = std::minmax(idA, idB);

                        newCollisionIds.insert(idPair);

                        const bool isNewCollision =
                            previousCollisionIds_.find(idPair) ==
                            previousCollisionIds_.end();

                        // Dispatch collision events and resolve physics
                        ResolveCollision(
                            *objA,
                            *objB,
                            *colliderA,
                            *colliderB,
                            isNewCollision,
                            manifold);

                        // Callbacks may destroy the objects
                        if (objA->IsDestroyed() || objB->IsDestroyed() ||
                            !objA->IsEnabled() || !objB->IsEnabled())
                        {
                            stopPair = true;
                            break;
                        }
                    }

                    if (stopPair)
                        break;
                }

                // Remove destroyed objects from the event lookup
                if (objA->IsDestroyed())
                    idToObject.erase(idA);

                if (objB->IsDestroyed())
                    idToObject.erase(idB);

                if (objA->IsDestroyed() || !objA->IsEnabled())
                    break;
            }
        }

        // Dispatch Exit events for pairs no longer colliding
        DetectCollisionEvents(idToObject, newCollisionIds);

        // Save the collision state for the next physics step
        previousCollisionIds_ = std::move(newCollisionIds);
    }

    void PhysicsManager::ProcessForces(
        const std::vector<GameObject *> &objects,
        float dt)
    {
        Scene *scene = ctx_.sceneProvider->GetCurrentScene();
        if (!scene)
            return;

        const glm::vec3 gravity = scene->Gravity;

        for (GameObject *obj : objects)
        {
            if (!obj || obj->IsDestroyed() || !obj->IsEnabled())
                continue;

            RigidBody *rigidBody = obj->GetComponent<RigidBody>();

            if (!rigidBody)
                continue;

            rigidBody->IntegrateForces(dt, gravity);
            rigidBody->IntegrateVelocity(dt);
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

        const auto effectiveInverseMass = [](const RigidBody* body)
        {
            if (!body || body->IsStatic() || body->GetMass() <= 0.0f)
                return glm::vec3(0.0f);

            const float inverseMass = 1.0f / body->GetMass();
            const RigidbodyLock locks = body->GetLocks();
            return glm::vec3(
                (locks & Lock_PosX) ? 0.0f : inverseMass,
                (locks & Lock_PosY) ? 0.0f : inverseMass,
                (locks & Lock_PosZ) ? 0.0f : inverseMass);
        };

        const glm::vec3 invMassA = effectiveInverseMass(rigidA);
        const glm::vec3 invMassB = effectiveInverseMass(rigidB);
        const glm::vec2 normal{manifold.normal.x, manifold.normal.y};

        const float inverseMassAlongNormal =
            invMassA.x * normal.x * normal.x +
            invMassA.y * normal.y * normal.y +
            invMassB.x * normal.x * normal.x +
            invMassB.y * normal.y * normal.y;

        if (inverseMassAlongNormal <= 0.0f)
            return;

        // Separate objects based on penetration depth
        float percent = 0.8f; // Penetration percentage to correct (0.2 to 1.0)
        float slop = 0.01f;   // Penetration allowance
        const float correctionMagnitude =
            glm::max(manifold.penetration - slop, 0.0f) /
            inverseMassAlongNormal * percent;
        const glm::vec2 correction = correctionMagnitude * normal;

        if (rigidA)
        {
            glm::vec3 posA = objA.GetTransform()->GetWorldPosition();
            posA.x -= correction.x * invMassA.x;
            posA.y -= correction.y * invMassA.y;
            objA.GetTransform()->SetPosition(posA);
        }

        if (rigidB)
        {
            glm::vec3 posB = objB.GetTransform()->GetWorldPosition();

            posB.x += correction.x * invMassB.x;
            posB.y += correction.y * invMassB.y;
            objB.GetTransform()->SetPosition(posB);
        }

        // Apply impulse for velocity correction
        glm::vec3 relativeVelocity = glm::vec3(0.0f);
        if (rigidA)
        {
            const glm::vec3 velocity = rigidA->GetVelocity();
            if (invMassA.x > 0.0f)
                relativeVelocity.x -= velocity.x;
            if (invMassA.y > 0.0f)
                relativeVelocity.y -= velocity.y;
        }
        if (rigidB)
        {
            const glm::vec3 velocity = rigidB->GetVelocity();
            if (invMassB.x > 0.0f)
                relativeVelocity.x += velocity.x;
            if (invMassB.y > 0.0f)
                relativeVelocity.y += velocity.y;
        }

        const float velocityAlongNormal =
            glm::dot(glm::vec2(relativeVelocity.x, relativeVelocity.y), normal);

        // Don't resolve if velocities are separating
        if (velocityAlongNormal > 0)
            return;

        // Calculate restitution (bounciness)
        float restitution = 0.2f; // 0 = inelastic, 1 = perfectly elastic

        // Calculate impulse scalar
        float impulseScalar = -(1.0f + restitution) * velocityAlongNormal;
        impulseScalar /= inverseMassAlongNormal;

        // Apply impulse
        const glm::vec2 impulse = impulseScalar * normal;

        if (rigidA)
        {
            glm::vec3 velA = rigidA->GetVelocity();
            velA.x -= impulse.x * invMassA.x;
            velA.y -= impulse.y * invMassA.y;
            rigidA->SetVelocity(velA);
        }

        if (rigidB)
        {
            glm::vec3 velB = rigidB->GetVelocity();
            velB.x += impulse.x * invMassB.x;
            velB.y += impulse.y * invMassB.y;
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
