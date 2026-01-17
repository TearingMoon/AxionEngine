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

            Manifold manifold;
            if (colliderA->Intersects(*colliderB, manifold))
            {
                currentCollisions_.insert(pair);
                bool isNewCollision = previousCollisions_.find(pair) == previousCollisions_.end();
                ResolveCollision(*objA, *objB, *colliderA, *colliderB, isNewCollision, manifold);
            }
        }
    }

    DetectCollisionEvents();
    previousCollisions_ = currentCollisions_;
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
