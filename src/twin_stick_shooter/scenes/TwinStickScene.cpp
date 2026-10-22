#include "TwinStickScene.hpp"
#include "../scripts/InfiniteBackgroundScript.hpp"

namespace Axion
{

    void TwinStickScene::OnSceneEnter()
    {
        // Disable gravity for top-down 2D gameplay
        SetGravity({0.0f, 0.0f, 0.0f});

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
                tileRenderer->SetLayer(RenderLayer::Background);
                tileRenderer->SetTexture(bgTexture);
                infiniteBg->tiles.push_back(tile);
            }
        }

        // Player GameObject
        player = CreateGameObject();
        player->GetTransform()->SetPosition({0.0f, 0.0f, 1.0f});
        auto playerRenderer = player->AddComponent<SpriteRenderComponent>();
        playerRenderer->SetTexture(assets().LoadTexture("PlayerSprite", "survivor_pistol.png"));
        playerRenderer->SetLayer(RenderLayer::Default);
        player->GetTransform()->SetScale({100.0f, 100.0f, 1.0f});
        auto playerScript = player->AddComponent<PlayerScript>();
        playerScript->scene = this;

        // Setup player callbacks
        playerScript->onHealthChanged = [this](int health)
        {
            if (gameUI)
                gameUI->SetHealth(health);
        };
        playerScript->onDeath = [this]()
        {
            OnPlayerDeath();
        };

        // Player Rigidbody and physical collider for movement interactions
        auto playerRigidbody = player->AddComponent<RigidBody>();
        playerRigidbody->SetGravityScale(0.0f);
        playerRigidbody->mass = 1.0f;
        auto playerCollider = player->AddComponent<CircleColliderComponent>();
        playerCollider->SetRadius(0.4f);
        playerCollider->SetColliderType(ColliderType::Physics);

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
        crosshair->GetTransform()->SetPosition({100.0f, 100.0f, 1.0f});
        auto crosshairRenderer = crosshair->AddComponent<SpriteRenderComponent>();
        crosshairRenderer->SetTexture(assets().LoadTexture("Crosshair", "Reticle-66.png"));
        crosshairRenderer->SetLayer(RenderLayer::UI);
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

        auto canvas = uiController->AddComponent<Canvas>();
        auto panel = canvas->Add<UIPanel>();
        panel->SetFillParent(true);
        panel->SetColor({0, 0, 0, 0});

        // Score and health labels remain screen-space and independent of the camera.
        auto scoreText = panel->Add<UILabel>("Score: 0");
        scoreText->SetPosition({24.0f, 20.0f});
        scoreText->SetColor({255, 255, 255, 255});
        scoreText->SetFontSize(24);
        gameUI->scoreText = scoreText;

        auto healthText = panel->Add<UILabel>("Health: 100");
        healthText->SetPosition({24.0f, 56.0f});
        healthText->SetColor({255, 100, 100, 255});
        healthText->SetFontSize(24);
        gameUI->healthText = healthText;

        auto roundText = panel->Add<UILabel>("Round: 1");
        roundText->SetAnchor({1.0f, 0.0f});
        roundText->SetPosition({-180.0f, 20.0f});
        roundText->SetColor({255, 255, 0, 255});
        roundText->SetFontSize(24);
        gameUI->roundText = roundText;

        auto gameOverText = panel->Add<UILabel>("GAME OVER");
        gameOverText->SetAnchor({0.5f, 0.5f});
        gameOverText->SetPosition({-130.0f, -60.0f});
        gameOverText->SetColor({255, 0, 0, 255});
        gameOverText->SetFontSize(48);
        gameOverText->SetVisible(false);
        gameUI->gameOverText = gameOverText;

        auto restartButton = panel->Add<UIButton>("Restart");
        restartButton->SetAnchor({0.5f, 0.5f});
        restartButton->SetPosition({-80.0f, 10.0f});
        restartButton->SetSize({160.0f, 48.0f});
        restartButton->SetVisible(false);
        restartButton->SetOnClick([this]() { RestartGame(); });
        gameUI->restartButton = restartButton;
    }

    void TwinStickScene::OnSceneUpdate()
    {
        // Check for restart
        if (isGameOver && ctx_.inputProvider->IsKeyJustPressed(SDL_SCANCODE_R))
        {
            RestartGame();
            return;
        }

        if (isGameOver)
            return;

        // Spawn zombies for current round
        if (zombiesSpawnedThisRound < zombiesPerRound)
        {
            zombieSpawnTimer += ctx_.timeProvider->GetDeltaTime();
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
        zombiesPerRound = 5 + (currentRound - 1) * 3;                           // 5, 8, 11, 14...
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
            0.0f);

        auto zombie = CreateGameObject();
        zombie->GetTransform()->SetPosition(spawnPos);
        zombie->GetTransform()->SetScale({80.0f, 80.0f, 1.0f});

        // Render zombie with sprite
        auto zombieRenderer = zombie->AddComponent<SpriteRenderComponent>();
        zombieRenderer->SetTexture(assets().LoadTexture("ZombieSprite", "zombie.png"));

        // Add physics body and collider for physical zombie/player collisions
        auto zombieRigidbody = zombie->AddComponent<RigidBody>();
        zombieRigidbody->SetGravityScale(0.0f);
        auto zombieCollider = zombie->AddComponent<CircleColliderComponent>();
        zombieCollider->SetRadius(0.5f);
        zombieCollider->SetColliderType(ColliderType::Physics);

        // Add AI script with death callback
        auto zombieScript = zombie->AddComponent<ZombieScript>();

        // Increase zombie speed each round
        zombieScript->moveSpeed = 80.0f + (currentRound - 1) * 10.0f;

        // Set callback for when zombie dies
        zombieScript->onDeath = [this](int points)
        {
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
        ctx_.sceneProvider->RequestChange("TwinStickScene");
    }

} // namespace Axion
