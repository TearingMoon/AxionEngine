#pragma once

#include <glm/glm.hpp>

#include "axion_engine/runtime/components/Component.hpp"

namespace Axion
{

/**
 * @brief Flags for constraining rigidbody movement.
 */
enum RigidbodyLock : uint32_t
{
    Lock_None = 0,

    Lock_PosX = 1u << 0,    ///< Lock X position
    Lock_PosY = 1u << 1,    ///< Lock Y position
    Lock_PosZ = 1u << 2,    ///< Lock Z position

    Lock_RotX = 1u << 3,    ///< Lock X rotation
    Lock_RotY = 1u << 4,    ///< Lock Y rotation
    Lock_RotZ = 1u << 5,    ///< Lock Z rotation

    Lock_AllPos = Lock_PosX | Lock_PosY | Lock_PosZ,    ///< Lock all positions
    Lock_AllRot = Lock_RotX | Lock_RotY | Lock_RotZ,    ///< Lock all rotations
    Lock_All = Lock_AllPos | Lock_AllRot                 ///< Lock everything
};

/**
 * @brief Component for physics-based movement.
 * 
 * RigidBody provides velocity-based movement with force accumulation,
 * gravity, and drag. Use with ColliderComponent for full physics interaction.
 */
class RigidBody : public Component
{
public:
    float mass = 1.0f;      ///< Mass in kilograms
    float drag = 0.0f;      ///< Linear drag coefficient (0 = no drag)

    /**
     * @brief Adds a force to be applied next physics update.
     * @param force Force vector in Newtons
     */
    void AddForce(const glm::vec3& force)
    {
        accumulatedForce_ += force;
    }

    /**
     * @brief Adds a force to be applied next physics update.
     * @param x X component of force
     * @param y Y component of force
     * @param z Z component of force
     */
    void AddForce(float x, float y, float z)
    {
        accumulatedForce_ += glm::vec3(x, y, z);
    }

    /**
     * @brief Integrates accumulated forces into velocity.
     * @param dt Delta time
     * @param gravity Gravity vector to apply
     */
    void IntegrateForces(float dt, const glm::vec3& gravity = glm::vec3(0.0f))
    {
        glm::vec3 acceleration = accumulatedForce_ / mass;
        acceleration += gravity;
        velocity_ += acceleration * dt;
        
        if (drag > 0.0f)
            velocity_ *= (1.0f - drag * dt);
        
        accumulatedForce_ = glm::vec3(0.0f);
    }

    /**
     * @brief Integrates velocity into position.
     * @param dt Delta time
     */
    void IntegrateVelocity(float dt)
    {
        glm::vec3 deltaPos = velocity_ * dt;

        if (!(locks_ & Lock_PosX))
            GetOwner()->GetTransform()->Translate(deltaPos.x, 0.0f, 0.0f);
        if (!(locks_ & Lock_PosY))
            GetOwner()->GetTransform()->Translate(0.0f, deltaPos.y, 0.0f);
        if (!(locks_ & Lock_PosZ))
            GetOwner()->GetTransform()->Translate(0.0f, 0.0f, deltaPos.z);
    }

    /** @brief Sets the velocity directly. */
    void SetVelocity(const glm::vec3& vel) { velocity_ = vel; }
    
    /** @brief Sets movement constraint flags. */
    void SetLocks(RigidbodyLock lockFlags) { locks_ = lockFlags; }
    
    /** @brief Returns the mass. */
    float GetMass() const { return mass; }
    
    /** @brief Returns the current velocity. */
    glm::vec3 GetVelocity() const { return velocity_; }
    
    /** @brief Returns the movement constraint flags. */
    RigidbodyLock GetLocks() const { return locks_; }

private:
    glm::vec3 velocity_ = glm::vec3(0.0f);
    glm::vec3 accumulatedForce_ = glm::vec3(0.0f);
    RigidbodyLock locks_ = RigidbodyLock::Lock_None;
};

} // namespace Axion