#pragma once

#include "axion_engine/runtime/components/scriptable/ScriptableComponent.hpp"
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

class PlayerScript : public ScriptableComponent
{
public:
    float moveSpeed = 200.0f;
    float sprintMultiplier = 1.8f;
    float shootCooldown = 0.15f;
    Scene *scene = nullptr;

    void Start(EngineContext &context) override
    {
        lastShootTime = 0.0f;
    }

    void Update(EngineContext &context) override
    {
        auto input = context.input;
        auto window = context.window;
        auto tr = GetOwner()->GetTransform();
        float deltaTime = context.time->GetDeltaTime();

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

    void ShootBullet(EngineContext &context, const glm::vec3 &position, const glm::vec2 &direction)
    {
        if (!scene)
            return;

        // Create bullet GameObject
        auto bullet = scene->CreateGameObject();
        bullet->GetTransform()->SetPosition(position);
        bullet->GetTransform()->SetScale({20.0f, 20.0f, 1.0f});

        // Add visual component (circle)
        auto bulletRenderer = bullet->AddComponent<CircleRenderComponent>();
        bulletRenderer->SetRadius(1.0f);
        bulletRenderer->SetColor({255, 255, 0, 255}); // Yellow

        // Add collider
        auto bulletCollider = bullet->AddComponent<CircleColliderComponent>();
        bulletCollider->SetRadius(1.0f);
        bulletCollider->SetColliderType(COLLIDER_TYPE::TRIGGER);

        // Add bullet script and set direction
        auto bulletScript = bullet->AddComponent<BulletScript>();
        bulletScript->direction = direction;
    }
};