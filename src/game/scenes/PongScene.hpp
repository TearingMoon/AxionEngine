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

    GameObject* player1ScoreText = nullptr;
    GameObject* player2ScoreText = nullptr;
    GameObject* ballObject = nullptr;
    
    int player1Score = 0;
    int player2Score = 0;
};
