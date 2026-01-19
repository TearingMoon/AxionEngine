#include "TwinStickScene.hpp"
#include "../scripts/InfiniteBackgroundScript.hpp"

namespace Axion
{

void TwinStickScene::OnSceneEnter()
{
    // Reset game state
    isGameOver = false;
    currentRound = 1;
    totalScore = 0;
    zombiesKilledThisRound = 0;
    zombiesSpawnedThisRound = 0;
    zombieSpawnTimer = 0.0f;
    
    // Infinite Background System - 9 tiles in a 3x3 grid
    const float tileSize = 1024.0f;
    auto bgController = CreateGameObject();
    auto infiniteBg = bgController->AddComponent<InfiniteBackgroundScript>();
    infiniteBg->tileSize = tileSize;
    
    // Load background texture once
    auto bgTexture = assets().LoadTexture("Background", "background.png");
    
    // Create 9 background tiles
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            auto tile = CreateGameObject();
            tile->GetTransform()->SetPosition({x * tileSize, y * tileSize, -0.5f});
            tile->GetTransform()->SetScale({tileSize, tileSize, 1.0f});
            auto tileRenderer = tile->AddComponent<SpriteRenderComponent>();
            tileRenderer->SetTexture(bgTexture);
            infiniteBg->tiles.push_back(tile);
        }
    }

    // Player GameObject
    player = CreateGameObject();
    player->GetTransform()->SetPosition({0.0f, 0.0f, 0.0f});
    auto playerRenderer = player->AddComponent<SpriteRenderComponent>();
    playerRenderer->SetTexture(assets().LoadTexture("PlayerSprite", "survivor_pistol.png"));
    player->GetTransform()->SetScale({100.0f, 100.0f, 1.0f});
    auto playerScript = player->AddComponent<PlayerScript>();
    playerScript->scene = this;
    
    // Setup player callbacks
    playerScript->onHealthChanged = [this](int health) {
        if (gameUI) gameUI->SetHealth(health);
    };
    playerScript->onDeath = [this]() {
        OnPlayerDeath();
    };
    
    auto playerCollider = player->AddComponent<CircleColliderComponent>();
    playerCollider->SetRadius(0.4f);
    playerCollider->SetColliderType(ColliderType::Trigger);

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

    // Create UI
    CreateUI();

    // Start first round
    StartNextRound();
}

void TwinStickScene::CreateUI()
{
    // UI Controller GameObject
    auto uiController = CreateGameObject();
    gameUI = uiController->AddComponent<GameUIScript>();
    
    // Score Text
    auto scoreGO = CreateGameObject();
    scoreGO->GetTransform()->SetPosition({-480.0f, 350.0f, 0.9f});
    auto scoreText = scoreGO->AddComponent<TextRenderComponent>();
    scoreText->SetText("Score: 0");
    scoreText->SetColor({255, 255, 255, 255});
    scoreText->SetFontSize(24);
    gameUI->scoreText = scoreText;
    
    // Health Text
    auto healthGO = CreateGameObject();
    healthGO->GetTransform()->SetPosition({-480.0f, 310.0f, 0.9f});
    auto healthText = healthGO->AddComponent<TextRenderComponent>();
    healthText->SetText("Health: 100");
    healthText->SetColor({255, 100, 100, 255});
    healthText->SetFontSize(24);
    gameUI->healthText = healthText;
    
    // Round Text
    auto roundGO = CreateGameObject();
    roundGO->GetTransform()->SetPosition({350.0f, 350.0f, 0.9f});
    auto roundText = roundGO->AddComponent<TextRenderComponent>();
    roundText->SetText("Round: 1");
    roundText->SetColor({255, 255, 0, 255});
    roundText->SetFontSize(24);
    gameUI->roundText = roundText;
    
    // Game Over Text (hidden initially)
    auto gameOverGO = CreateGameObject();
    gameOverGO->GetTransform()->SetPosition({0.0f, 0.0f, 0.95f});
    auto gameOverText = gameOverGO->AddComponent<TextRenderComponent>();
    gameOverText->SetText("");
    gameOverText->SetColor({255, 0, 0, 255});
    gameOverText->SetFontSize(48);
    gameUI->gameOverText = gameOverText;
}

void TwinStickScene::OnSceneUpdate()
{
    // Check for restart
    if (isGameOver && ctx_.input->IsKeyJustPressed(SDL_SCANCODE_R))
    {
        RestartGame();
        return;
    }
    
    if (isGameOver) return;
    
    // Spawn zombies for current round
    if (zombiesSpawnedThisRound < zombiesPerRound)
    {
        zombieSpawnTimer += ctx_.time->GetDeltaTime();
        if (zombieSpawnTimer >= zombieSpawnInterval)
        {
            SpawnZombie();
            zombieSpawnTimer = 0.0f;
        }
    }
    
    // Check if round is complete
    if (zombiesKilledThisRound >= zombiesPerRound)
    {
        currentRound++;
        StartNextRound();
    }
}

void TwinStickScene::StartNextRound()
{
    zombiesKilledThisRound = 0;
    zombiesSpawnedThisRound = 0;
    zombieSpawnTimer = 0.0f;
    
    // Increase difficulty each round
    zombiesPerRound = 5 + (currentRound - 1) * 3; // 5, 8, 11, 14...
    zombieSpawnInterval = std::max(0.5f, 1.5f - (currentRound - 1) * 0.1f); // Faster spawns
    
    if (gameUI)
    {
        gameUI->SetRound(currentRound);
    }
}

void TwinStickScene::SpawnZombie()
{
    if (!player || player->IsDestroyed() || isGameOver)
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
    zombieCollider->SetColliderType(ColliderType::Trigger);

    // Add AI script with death callback
    auto zombieScript = zombie->AddComponent<ZombieScript>();
    
    // Increase zombie speed each round
    zombieScript->moveSpeed = 80.0f + (currentRound - 1) * 10.0f;
    
    // Set callback for when zombie dies
    zombieScript->onDeath = [this](int points) {
        AddScore(points);
        zombiesKilledThisRound++;
    };

    zombiesSpawnedThisRound++;
}

void TwinStickScene::AddScore(int points)
{
    totalScore += points;
    if (gameUI)
    {
        gameUI->AddScore(points);
    }
}

void TwinStickScene::OnPlayerDeath()
{
    isGameOver = true;
    if (gameUI)
    {
        gameUI->ShowGameOver();
    }
}

void TwinStickScene::RestartGame()
{
    // Reload the scene
    ctx_.scene->RequestChange("TwinStickScene");
}

} // namespace Axion
