#pragma once

#include "axion_engine/Axion.hpp"
#include "pong/scripts/PlayerPaddle.hpp"
#include "pong/scripts/AIPaddle.hpp"
#include "pong/scripts/Ball.hpp"

namespace Axion
{

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

} // namespace Axion
