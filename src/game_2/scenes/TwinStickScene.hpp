#pragma once

#include "axion_engine/managers/assets/AssetsManager.hpp"

#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"
#include "axion_engine/runtime/components/renderer/SquareRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/CircleRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/TextRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/SpriteRenderComponent.hpp"

#include "axion_engine/runtime/components/collider/CircleColliderComponent.hpp"

#include "game_2/scripts/ReticleScript.hpp"
#include "game_2/scripts/PlayerScript.hpp"
#include "game_2/scripts/ZombieScript.hpp"
#include "game_2/scripts/BulletScript.hpp"
#include "game_2/scripts/CameraFollowScript.hpp"
#include "game_2/scripts/GameUIScript.hpp"

class TwinStickScene : public Scene
{
public:
    using Scene::Scene;

    void OnSceneEnter() override;

    void OnSceneUpdate() override;

    void OnSceneExit() override
    {
        printf("Exiting Twin Stick Scene.\n");
    }
    
    // Game state
    void AddScore(int points);
    void OnPlayerDeath();
    void RestartGame();

private:
    GameObject *player = nullptr;
    GameUIScript *gameUI = nullptr;
    
    // Round system
    int currentRound = 1;
    int zombiesPerRound = 5;
    int zombiesKilledThisRound = 0;
    int zombiesSpawnedThisRound = 0;
    float zombieSpawnTimer = 0.0f;
    float zombieSpawnInterval = 1.5f;
    
    // Game state
    bool isGameOver = false;
    int totalScore = 0;

    void SpawnZombie();
    void StartNextRound();
    void CreateUI();
};
