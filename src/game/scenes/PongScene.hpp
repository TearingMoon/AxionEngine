#pragma once

#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"
#include "axion_engine/runtime/components/renderer/SquareRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/TextRenderComponent.hpp"
#include "axion_engine/runtime/components/collider/AABBColliderComponent.hpp"

#include "game/scripts/PlayerPaddle.hpp"
#include "game/scripts/AIPaddle.hpp"
#include "game/scripts/Ball.hpp"

class PongScene : public Scene
{
public:
    using Scene::Scene;

    void OnSceneEnter() override;
    void OnSceneUpdate() override;
    void OnSceneExit() override;

private:
    void UpdateScore();
    void Player1Scored();
    void Player2Scored();
    void ResetGame();

    GameObject* player1ScoreText = nullptr;
    GameObject* player2ScoreText = nullptr;
    GameObject* ballObject = nullptr;
    GameObject* winnerText = nullptr;
    
    int player1Score = 0;
    int player2Score = 0;
    
    bool gameOver = false;
    float gameOverTimer = 0.0f;
    const float restartDelay = 3.0f;
};
