#pragma once

#include <glm/glm.hpp>

#include "axion_engine/runtime/components/Component.hpp"

enum RigidbodyLock : uint32_t
{
    Lock_None = 0,

    Lock_PosX = 1u << 0,
    Lock_PosY = 1u << 1,
    Lock_PosZ = 1u << 2,

    Lock_RotX = 1u << 3,
    Lock_RotY = 1u << 4,
    Lock_RotZ = 1u << 5,

    Lock_AllPos = Lock_PosX | Lock_PosY | Lock_PosZ,
    Lock_AllRot = Lock_RotX | Lock_RotY | Lock_RotZ,
    Lock_All = Lock_AllPos | Lock_AllRot
};

class RigidBody : public Component
{
public:
    float mass = 1.0f;
    float drag = 0.0f; // Air resistance (0 = no drag)

    void AddForce(const glm::vec3 &force)
    {
        accumulatedForce_ += force;
    }

    void AddForce(float x, float y, float z)
    {
        accumulatedForce_ += glm::vec3(x, y, z);
    }

    void IntegrateForces(float dt, const glm::vec3 &gravity = glm::vec3(0.0f))
    {
        // Calculate acceleration: F = ma -> a = F/m
        glm::vec3 acceleration = accumulatedForce_ / mass;
        
        // Add gravity
        acceleration += gravity;
        
        // Update velocity: v = v0 + a*dt
        velocity_ += acceleration * dt;
        
        // Apply drag (simple linear damping)
        if (drag > 0.0f)
        {
            velocity_ *= (1.0f - drag * dt);
        }
        
        // Clear accumulated forces for next frame
        accumulatedForce_ = glm::vec3(0.0f);
    }

    void IntegrateVelocity(float dt)
    {
        // Calculate position delta: p = p0 + v*dt
        glm::vec3 deltaPos = velocity_ * dt;

        // Apply position changes respecting locks
        if (!(locks_ & Lock_PosX))
            GetOwner()->GetTransform()->Translate(deltaPos.x, 0.0f, 0.0f);
        if (!(locks_ & Lock_PosY))
            GetOwner()->GetTransform()->Translate(0.0f, deltaPos.y, 0.0f);
        if (!(locks_ & Lock_PosZ))
            GetOwner()->GetTransform()->Translate(0.0f, 0.0f, deltaPos.z);
    }

    void SetVelocity(const glm::vec3 &vel) { velocity_ = vel; }
    void SetLocks(RigidbodyLock lockFlags) { locks_ = lockFlags; }
    
    float GetMass() const { return mass; }
    glm::vec3 GetVelocity() const { return velocity_; }
    RigidbodyLock GetLocks() const { return locks_; }

private:
    glm::vec3 velocity_ = glm::vec3(0.0f);
    glm::vec3 accumulatedForce_ = glm::vec3(0.0f);
    RigidbodyLock locks_ = RigidbodyLock::Lock_None;
};