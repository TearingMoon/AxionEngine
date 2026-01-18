#include "TwinStickScene.hpp"

void TwinStickScene::OnSceneEnter()
{
    // Background/Floor
    auto floor = CreateGameObject();
    floor->GetTransform()->SetPosition({0.0f, 0.0f, -0.5f});
    floor->GetTransform()->SetScale({2000.0f, 2000.0f, 1.0f});
    auto floorRenderer = floor->AddComponent<SquareRenderComponent>();
    floorRenderer->SetColor({40, 50, 40, 255}); // Dark green ground

    // Player GameObject
    player = CreateGameObject();
    player->GetTransform()->SetPosition({0.0f, 0.0f, 0.0f});
    auto playerRenderer = player->AddComponent<SpriteRenderComponent>();
    playerRenderer->SetTexture(assets().LoadTexture("PlayerSprite", "survivor_pistol.png"));
    player->GetTransform()->SetScale({100.0f, 100.0f, 1.0f});
    auto playerScript = player->AddComponent<PlayerScript>();
    playerScript->scene = this; // Give player reference to scene for spawning bullets
    auto playerCollider = player->AddComponent<CircleColliderComponent>();
    playerCollider->SetRadius(0.4f);
    playerCollider->SetColliderType(COLLIDER_TYPE::TRIGGER);

    // Camera GameObject (created after player so it can follow)
    auto cameraGO = CreateGameObject();
    auto cameraComponent = cameraGO->AddComponent<CameraComponent>();
    cameraComponent->SetOrthographic(1024.0f, 768.0f, -1.0f, 1.0f);
    SetCurrentCamera(cameraComponent);
    auto cameraFollow = cameraGO->AddComponent<CameraFollowScript>();
    cameraFollow->target = player;
    cameraFollow->smoothSpeed = 5.0f;

    // Crosshair GameObject
    auto crosshair = CreateGameObject();
    crosshair->GetTransform()->SetPosition({100.0f, 100.0f, 0.0f});
    auto crosshairRenderer = crosshair->AddComponent<SpriteRenderComponent>();
    crosshairRenderer->SetTexture(assets().LoadTexture("Crosshair", "Reticle-66.png"));
    crosshair->GetTransform()->SetScale({100.0f, 100.0f, 1.0f});
    auto reticleScript = crosshair->AddComponent<ReticleScript>();

    // Spawn initial zombies
    for (int i = 0; i < 3; i++)
    {
        SpawnZombie();
    }
}

void TwinStickScene::OnSceneUpdate()
{
    // Spawn zombies periodically
    if (zombiesSpawned < maxZombies)
    {
        zombieSpawnTimer += ctx_.time->GetDeltaTime();
        if (zombieSpawnTimer >= zombieSpawnInterval)
        {
            SpawnZombie();
            zombieSpawnTimer = 0.0f;
        }
    }
}

void TwinStickScene::SpawnZombie()
{
    // Validate player is not null and not destroyed
    if (!player || player->IsDestroyed() || zombiesSpawned >= maxZombies)
        return;

    // Spawn zombie at random position around player
    float angle = (rand() % 360) * 3.14159f / 180.0f;
    float distance = 400.0f + (rand() % 200);
    glm::vec3 playerPos = player->GetTransform()->GetPosition();
    glm::vec3 spawnPos(
        playerPos.x + cos(angle) * distance,
        playerPos.y + sin(angle) * distance,
        0.0f
    );

    auto zombie = CreateGameObject();
    zombie->GetTransform()->SetPosition(spawnPos);
    zombie->GetTransform()->SetScale({80.0f, 80.0f, 1.0f});
    
    // Render zombie with sprite
    auto zombieRenderer = zombie->AddComponent<SpriteRenderComponent>();
    zombieRenderer->SetTexture(assets().LoadTexture("ZombieSprite", "zombie.png"));

    // Add collider
    auto zombieCollider = zombie->AddComponent<CircleColliderComponent>();
    zombieCollider->SetRadius(0.5f);
    zombieCollider->SetColliderType(COLLIDER_TYPE::TRIGGER);

    // Add AI script
    auto zombieScript = zombie->AddComponent<ZombieScript>();
    // No need to pass player reference anymore

    zombiesSpawned++;
}
