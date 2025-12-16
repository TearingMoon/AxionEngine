#pragma once

#include <cstdio>
#include <algorithm>
#include <vector>

#include "axion_engine/structure/ContextAware.hpp"
#include "axion_engine/core/EngineContext.hpp"
#include "axion_engine/managers/time/TimeManager.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"

#include "axion_engine/runtime/components/collider/ColliderComponent.hpp"
#include "axion_engine/runtime/components/collider/SphereColliderComponent.hpp"

#include "axion_utilities/vector_filter/PtrVectorFilter.hpp"

#include "PhysicsData.hpp"

class PhysicsManager : public ContextAware
{
public:
    PhysicsManager(EngineContext &ctx);
    ~PhysicsManager();

    void Update(float dt);

private:
    float fixedDeltaTime_ = 0.02f;
    float accumulatedTime_ = 0.0f;

    std::unordered_set<std::pair<ColliderComponent *, ColliderComponent *>, PairHash> currentCollisions_;
    std::unordered_set<std::pair<ColliderComponent *, ColliderComponent *>, PairHash> previousCollisions_;

    void FixedUpdate(float dt);
    void ProcessCollisions(std::vector<GameObject *> &gameObjectsWithCollider);

    bool CheckCollision(ColliderComponent &colliderA, ColliderComponent &colliderB);

    bool CheckSphereSphereCollision(
        const SphereColliderComponent &sphereA,
        const SphereColliderComponent &sphereB,
        const TransformComponent &transformA,
        const TransformComponent &transformB);

    void ResolveCollision(
        GameObject &objA,
        GameObject &objB,
        ColliderComponent &colliderA,
        ColliderComponent &colliderB,
        bool isNewCollision);

    void DetectCollisionEvents();
};