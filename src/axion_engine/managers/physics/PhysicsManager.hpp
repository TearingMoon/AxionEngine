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
#include "axion_engine/runtime/components/collider/AABBColliderComponent.hpp"
#include "axion_engine/runtime/components/collider/OBBColliderComponent.hpp"
#include "axion_engine/runtime/components/collider/CircleColliderComponent.hpp"

#include "axion_engine/runtime/components/rigid_body/RigidBody.hpp"

#include "axion_utilities/vector_filter/PtrVectorFilter.hpp"

#include "PhysicsData.hpp"

namespace Axion
{

    /**
     * @brief Manages physics simulation including collision detection and response.
     *
     * PhysicsManager runs on a fixed timestep and handles:
     * - Collision detection between colliders (AABB, OBB, Circle)
     * - Collision event dispatch (Enter/Exit, Trigger/Physics)
     * - Rigidbody force integration and movement
     * - Gravity application
     */
    class PhysicsManager : public ContextAware
    {
    public:
        PhysicsManager(EngineContext &ctx);
        ~PhysicsManager();

        /**
         * @brief Updates the physics simulation.
         * @param dt Delta time since last frame
         */
        void Update(float dt);
        void SetFixedTimestep(float timestep);
        void SetMaxSubSteps(int maxSubSteps);

    private:
        float fixedDeltaTime_ = 1.0f / 60.0f;
        float accumulatedTime_ = 0.0f;
        int maxSubSteps_ = 8;

        std::vector<GameObject *> colliders_;
        std::vector<GameObject *> rigidBodies_;

        std::unordered_set<
            std::pair<ColliderComponent *, ColliderComponent *>,
            PairHash>
            currentCollisions_;

        std::unordered_set<
            std::pair<size_t, size_t>,
            PairHashSize>
            previousCollisionIds_;

        void FixedUpdate(float dt);

        void ProcessCollisions(const std::vector<GameObject *> &objects);
        void ProcessForces(const std::vector<GameObject *> &objects, float dt);
        void DetectCollisionEvents(const std::unordered_map<size_t, GameObject *> &idToObject,
                                   const std::unordered_set<std::pair<size_t, size_t>, PairHashSize> &currentCollisionIds);

        void ResolveCollision(
            GameObject &objA,
            GameObject &objB,
            ColliderComponent &colliderA,
            ColliderComponent &colliderB,
            bool isNewCollision,
            Manifold &manifold);
    };

} // namespace Axion