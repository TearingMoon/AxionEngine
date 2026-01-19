#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"
#include "axion_engine/runtime/components/rigid_body/RigidBody.hpp"
#include "axion_engine/platform/window/Window.hpp"
#include "axion_engine/managers/input/InputManager.hpp"
#include "axion_engine/managers/time/TimeManager.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/components/renderer/SpriteRenderComponent.hpp"
#include "axion_engine/runtime/components/collider/CircleColliderComponent.hpp"
#include "BulletScript.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <functional>

class PlayerScript : public ScriptableComponent
{
public:
    float moveSpeed = 200.0f;
    float sprintMultiplier = 1.8f;
    float shootCooldown = 0.25f;
    Scene *scene = nullptr;
    
    // Health system
    int maxHealth = 100;
    int currentHealth = 100;
    float damageCooldown = 1.0f; // Invulnerability time after taking damage
    bool isDead = false;
    
    // Callback for when health changes or player dies
    std::function<void(int health)> onHealthChanged = nullptr;
    std::function<void()> onDeath = nullptr;

    void Start(EngineContext &context) override
    {
        lastShootTime = 0.0f;
        lastDamageTime = damageCooldown; // Start able to take damage
        currentHealth = maxHealth;
        isDead = false;
    }
    
    void TakeDamage(int damage)
    {
        if (isDead || lastDamageTime < damageCooldown) return;
        
        currentHealth -= damage;
        lastDamageTime = 0.0f;
        
        if (onHealthChanged)
            onHealthChanged(currentHealth);
        
        if (currentHealth <= 0)
        {
            currentHealth = 0;
            isDead = true;
            if (onDeath)
                onDeath();
        }
    }

    void Update(EngineContext &context) override
    {
        auto *owner = GetOwner();
        if (!owner || owner->IsDestroyed()) return;
        
        auto *tr = owner->GetTransform();
        if (!tr) return;
        
        auto input = context.input;
        auto window = context.window;
        float deltaTime = context.time->GetDeltaTime();
        
        // Update damage cooldown
        lastDamageTime += deltaTime;
        
        // Don't allow input if dead
        if (isDead) return;

        // Sprint with Shift
        float currentSpeed = moveSpeed;
        if (input->IsKeyDown(SDL_SCANCODE_LSHIFT) || input->IsKeyDown(SDL_SCANCODE_RSHIFT))
        {
            currentSpeed *= sprintMultiplier;
        }

        // Movement with WASD
        glm::vec3 movement(0.0f);

        if (input->IsKeyDown(SDL_SCANCODE_W))
        {
            movement.y += 1.0f;
        }
        if (input->IsKeyDown(SDL_SCANCODE_S))
        {
            movement.y -= 1.0f;
        }
        if (input->IsKeyDown(SDL_SCANCODE_A))
        {
            movement.x -= 1.0f;
        }
        if (input->IsKeyDown(SDL_SCANCODE_D))
        {
            movement.x += 1.0f;
        }

        if (glm::length(movement) > 0.0f)
        {
            movement = glm::normalize(movement);
            glm::vec3 newPos = tr->GetPosition() + movement * currentSpeed * deltaTime;
            tr->SetPosition(newPos);
        }

        // Get player world position
        glm::vec3 playerPos = tr->GetPosition();

        // Get mouse position and convert to world coordinates
        SDL_Point mousePos = input->GetMousePosition();
        int winW = 0, winH = 0;
        SDL_GetWindowSize(window->GetSDLWindow(), &winW, &winH);

        // Get camera position to adjust world coordinates
        glm::vec3 cameraPos(0.0f);
        if (context.scene)
        {
            auto currentScene = context.scene->GetCurrentScene();
            if (currentScene)
            {
                auto camera = currentScene->GetCurrentCamera();
                if (camera)
                {
                    auto cameraTr = camera->GetOwner()->GetTransform();
                    if (cameraTr)
                    {
                        cameraPos = cameraTr->GetPosition();
                    }
                }
            }
        }

        // Convert screen to world coordinates (accounting for camera position)
        float mouseWorldX = (mousePos.x - winW / 2.0f) + cameraPos.x;
        float mouseWorldY = (winH / 2.0f - mousePos.y) + cameraPos.y;

        // Calculate direction from player to mouse in world space
        glm::vec2 direction(mouseWorldX - playerPos.x, mouseWorldY - playerPos.y);

        // Rotation towards mouse
        if (glm::length(direction) > 0.0f)
        {
            direction = glm::normalize(direction);
            float angle = atan2(direction.y, direction.x) * 180.0f / glm::pi<float>();
            tr->SetRotation({0.0f, 0.0f, angle - 90.0f});
        }

        // Shooting with mouse click
        lastShootTime += deltaTime;
        if (input->IsMouseButtonDown(SDL_BUTTON_LEFT) && lastShootTime >= shootCooldown && scene)
        {
            ShootBullet(context, playerPos, direction);
            lastShootTime = 0.0f;
        }
    }

private:
    float lastShootTime = 0.0f;
    float lastDamageTime = 0.0f;

    void ShootBullet(EngineContext &context, const glm::vec3 &position, const glm::vec2 &direction)
    {
        if (!scene)
            return;

        // Create bullet GameObject
        auto bullet = scene->CreateGameObject();
        bullet->GetTransform()->SetPosition(position);
        bullet->GetTransform()->SetScale({12.0f, 12.0f, 1.0f});

        // Add visual component (circle)
        auto bulletRenderer = bullet->AddComponent<CircleRenderComponent>();
        bulletRenderer->SetRadius(1.0f);
        bulletRenderer->SetColor({255, 255, 0, 255}); // Yellow

        // Add collider
        auto bulletCollider = bullet->AddComponent<CircleColliderComponent>();
        bulletCollider->SetRadius(1.0f);
        bulletCollider->SetColliderType(COLLIDER_TYPE::TRIGGER);

        // Add RigidBody for physics-based movement
        auto bulletRb = bullet->AddComponent<RigidBody>();
        bulletRb->mass = 0.1f;
        bulletRb->drag = 0.0f; // No drag for bullets

        // Add bullet script and set direction
        auto bulletScript = bullet->AddComponent<BulletScript>();
        bulletScript->direction = direction;
    }
};