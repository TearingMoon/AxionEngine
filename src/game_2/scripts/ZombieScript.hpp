#pragma once

#include "axion_engine/Axion.hpp"
#include "BulletScript.hpp"
#include <glm/glm.hpp>
#include <functional>

namespace Axion
{

// Forward declarations
class PlayerScript;

class ZombieScript : public ScriptableComponent
{
public:
    float moveSpeed = 80.0f;
    float health = 100.0f;
    int damageToPlayer = 20;
    int scoreValue = 100; // Points given when killed
    
    // Callback when zombie dies (for score)
    std::function<void(int points)> onDeath = nullptr;

    void Start(EngineContext &context) override
    {
    }

    void Update(EngineContext &context) override
    {
        auto *owner = GetOwner();
        if (!owner || owner->IsDestroyed()) return;
        
        auto *tr = owner->GetTransform();
        if (!tr) return;
        
        // Find player each frame instead of storing pointer
        GameObject *player = FindPlayer(context);
        if (!player)
            return;

        float deltaTime = context.time->GetDeltaTime();
        
        // Get direction to player
        glm::vec3 zombiePos = tr->GetPosition();
        
        auto *playerTr = player->GetTransform();
        if (!playerTr) return;
        
        glm::vec3 playerPos = playerTr->GetPosition();
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
            
        // Check if the other object is a bullet by checking for BulletScript
        auto bulletScript = other.GetComponent<BulletScript>();
        if (bulletScript)
        {
            // Hit by bullet - take damage and destroy the bullet
            TakeDamage(50.0f);
            other.Destroy();
            return;
        }
        
        // Check if it's the player - damage them
        auto playerScript = other.GetComponent<PlayerScript>();
        if (playerScript)
        {
            playerScript->TakeDamage(damageToPlayer);
        }
    }

    void TakeDamage(float damage)
    {
        auto *owner = GetOwner();
        if (!owner || owner->IsDestroyed()) return;
        
        health -= damage;
        if (health <= 0.0f)
        {
            // Notify score before dying
            if (onDeath)
                onDeath(scoreValue);
            
            owner->Destroy();
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

} // namespace Axion
