#pragma once

#include "axion_engine/Axion.hpp"
#include <glm/glm.hpp>

namespace Axion
{

class BulletScript : public ScriptableComponent
{
public:
    float speed = 500.0f;
    float lifetime = 3.0f; // Time in seconds before the bullet is destroyed
    glm::vec2 direction = glm::vec2(0.0f, 1.0f);
    bool isBullet = true; // Tag to identify this as a bullet

    void Start(EngineContext &context) override
    {
        currentLifetime = 0.0f;
        
        // Set initial velocity via RigidBody
        auto *owner = GetOwner();
        if (!owner) return;
        
        auto *rb = owner->GetComponent<RigidBody>();
        if (rb)
        {
            rb->SetVelocity(glm::vec3(direction.x * speed, direction.y * speed, 0.0f));
        }
    }

    void Update(EngineContext &context) override
    {
        auto *owner = GetOwner();
        if (!owner || owner->IsDestroyed()) return;
        
        float deltaTime = context.timeProvider->GetDeltaTime();

        // Check lifetime
        currentLifetime += deltaTime;
        if (currentLifetime >= lifetime)
        {
            owner->Destroy();
        }
    }

    void OnTriggerEnter(GameObject &other) override
    {
        // Bullet will be destroyed when it hits a zombie (handled by ZombieScript)
    }

private:
    float currentLifetime = 0.0f;
};

} // namespace Axion
