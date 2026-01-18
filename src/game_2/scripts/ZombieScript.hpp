#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include <glm/glm.hpp>

// Forward declarations
class BulletScript;
class PlayerScript;

class ZombieScript : public ScriptableComponent
{
public:
    float moveSpeed = 80.0f;
    float health = 100.0f;

    void Start(EngineContext &context) override
    {
    }

    void Update(EngineContext &context) override
    {
        // Find player each frame instead of storing pointer
        GameObject *player = FindPlayer(context);
        if (!player)
            return;

        float deltaTime = context.time->GetDeltaTime();
        auto tr = GetOwner()->GetTransform();
        
        // Get direction to player
        glm::vec3 zombiePos = tr->GetPosition();
        glm::vec3 playerPos = player->GetTransform()->GetPosition();
        glm::vec2 direction(playerPos.x - zombiePos.x, playerPos.y - zombiePos.y);

        // Move towards player
        if (glm::length(direction) > 0.0f)
        {
            direction = glm::normalize(direction);
            glm::vec3 newPos = zombiePos + glm::vec3(direction.x, direction.y, 0.0f) * moveSpeed * deltaTime;
            tr->SetPosition(newPos);

            // Rotate to face player
            float angle = atan2(direction.y, direction.x) * 180.0f / glm::pi<float>();
            tr->SetRotation({0.0f, 0.0f, angle - 90.0f});
        }
    }

    void OnTriggerEnter(GameObject &other) override
    {
        // Validate the other object is not already destroyed
        if (other.IsDestroyed())
            return;
            
        // Check if hit by bullet - simple collision detection
        // Assume any trigger collision damages the zombie
        TakeDamage(50.0f);
        other.Destroy();
    }

    void TakeDamage(float damage)
    {
        health -= damage;
        if (health <= 0.0f)
        {
            GetOwner()->Destroy();
        }
    }

private:
    GameObject *FindPlayer(EngineContext &context)
    {
        if (!context.scene)
            return nullptr;
            
        auto currentScene = context.scene->GetCurrentScene();
        if (!currentScene)
            return nullptr;
        
        // Find player by checking if it has PlayerScript component
        auto allObjects = currentScene->GetGameObjects();
        for (auto *obj : allObjects)
        {
            if (obj && !obj->IsDestroyed())
            {
                // Try to find PlayerScript component
                auto playerScript = obj->GetComponent<PlayerScript>();
                if (playerScript)
                {
                    return obj;
                }
            }
        }
        return nullptr;
    }
};
