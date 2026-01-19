#pragma once

#include "axion_engine/Axion.hpp"

#include "game_2/scripts/ReticleScript.hpp"
#include "game_2/scripts/PlayerScript.hpp"
#include "game_2/scripts/ZombieScript.hpp"
#include "game_2/scripts/BulletScript.hpp"
#include "game_2/scripts/CameraFollowScript.hpp"
#include "game_2/scripts/GameUIScript.hpp"

namespace Axion
{

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

} // namespace Axion
