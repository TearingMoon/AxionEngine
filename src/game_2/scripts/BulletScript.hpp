#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"
#include <glm/glm.hpp>

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
    }

    void Update(EngineContext &context) override
    {
        auto *owner = GetOwner();
        if (!owner || owner->IsDestroyed()) return;
        
        auto *tr = owner->GetTransform();
        if (!tr) return;
        
        float deltaTime = context.time->GetDeltaTime();
        
        // Move the bullet
        glm::vec3 currentPos = tr->GetPosition();
        currentPos.x += direction.x * speed * deltaTime;
        currentPos.y += direction.y * speed * deltaTime;
        tr->SetPosition(currentPos);

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
