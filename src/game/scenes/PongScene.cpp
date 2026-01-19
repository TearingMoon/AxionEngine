#include "PongScene.hpp"
#include <string>
#include <SDL.h>

namespace Axion
{

void PongScene::OnSceneEnter()
{
    printf("Entered Pong Scene - WASD to move, AI opponent!\n");

    int windowWidth, windowHeight;
    SDL_GetWindowSize(GetContext().window->GetSDLWindow(), &windowWidth, &windowHeight);

    const float screenWidth = static_cast<float>(windowWidth);
    const float screenHeight = static_cast<float>(windowHeight);

    const float paddleWidth = screenWidth * 0.02f;
    const float paddleHeight = screenHeight * 0.15f;
    const float ballSize = screenWidth * 0.02f;
    const float paddleOffset = screenWidth * 0.42f;

    auto cameraGO = CreateGameObject();
    auto cameraComponent = cameraGO->AddComponent<CameraComponent>();
    cameraComponent->SetOrthographic(screenWidth, screenHeight, -1.0f, 1.0f);
    SetCurrentCamera(cameraComponent);

    auto player1 = CreateGameObject();
    player1->GetTransform()->SetPosition({-paddleOffset, 0.0f, 0.0f});
    player1->GetTransform()->SetScale({paddleWidth, paddleHeight, 1.0f});
    auto p1Renderer = player1->AddComponent<SquareRenderComponent>();
    p1Renderer->SetColor({255, 255, 255, 255});
    auto p1Collider = player1->AddComponent<AABBColliderComponent>();
    p1Collider->SetColliderType(ColliderType::Physics);
    auto p1Script = player1->AddComponent<PlayerPaddle>();
    p1Script->SetBounds(-screenHeight / 2.0f + paddleHeight / 2.0f,
                        screenHeight / 2.0f - paddleHeight / 2.0f);
    p1Script->SetSpeed(400.0f);

    auto player2 = CreateGameObject();
    player2->GetTransform()->SetPosition({paddleOffset, 0.0f, 0.0f});
    player2->GetTransform()->SetScale({paddleWidth, paddleHeight, 1.0f});
    auto p2Renderer = player2->AddComponent<SquareRenderComponent>();
    p2Renderer->SetColor({255, 255, 255, 255});
    auto p2Collider = player2->AddComponent<AABBColliderComponent>();
    p2Collider->SetColliderType(ColliderType::Physics);
    auto p2Script = player2->AddComponent<AIPaddle>();
    p2Script->SetBounds(-screenHeight / 2.0f + paddleHeight / 2.0f,
                        screenHeight / 2.0f - paddleHeight / 2.0f);
    p2Script->SetSpeed(300.0f);

    ballObject = CreateGameObject();
    ballObject->GetTransform()->SetPosition({0.0f, 0.0f, 0.0f});
    ballObject->GetTransform()->SetScale({ballSize, ballSize, 1.0f});
    auto ballRenderer = ballObject->AddComponent<SquareRenderComponent>();
    ballRenderer->SetColor({255, 255, 100, 255});
    auto ballCollider = ballObject->AddComponent<AABBColliderComponent>();
    ballCollider->SetColliderType(ColliderType::Physics);
    auto ballScript = ballObject->AddComponent<Ball>();
    ballScript->SetBounds(-screenWidth / 2.0f, screenWidth / 2.0f,
                          -screenHeight / 2.0f + ballSize, screenHeight / 2.0f - ballSize);
    ballScript->SetInitialSpeed(280.0f);

    p2Script->SetBallTransform(ballObject->GetTransform());

    ballScript->SetOnPlayer1Score([this]() { Player1Scored(); });
    ballScript->SetOnPlayer2Score([this]() { Player2Scored(); });

    int lineCount = static_cast<int>(screenHeight / 40.0f);
    float lineSpacing = screenHeight / lineCount;
    float lineHeight = lineSpacing * 0.65f;
    for (int i = 0; i < lineCount; i++)
    {
        auto line = CreateGameObject();
        line->GetTransform()->SetPosition({0.0f, -screenHeight / 2.0f + i * lineSpacing + lineSpacing / 2.0f, 0.0f});
        line->GetTransform()->SetScale({screenWidth * 0.008f, lineHeight, 1.0f});
        auto lineRenderer = line->AddComponent<SquareRenderComponent>();
        lineRenderer->SetColor({100, 100, 100, 255});
    }

    player1ScoreText = CreateGameObject();
    player1ScoreText->GetTransform()->SetPosition({-screenWidth * 0.15f, screenHeight * 0.38f, 0.0f});
    auto p1Text = player1ScoreText->AddComponent<TextRenderComponent>();
    p1Text->SetText("0");
    p1Text->SetFontSize(static_cast<int>(screenHeight * 0.12f));

    player2ScoreText = CreateGameObject();
    player2ScoreText->GetTransform()->SetPosition({screenWidth * 0.1f, screenHeight * 0.38f, 0.0f});
    auto p2Text = player2ScoreText->AddComponent<TextRenderComponent>();
    p2Text->SetText("0");
    p2Text->SetFontSize(static_cast<int>(screenHeight * 0.12f));

    auto instructionsText = CreateGameObject();
    instructionsText->GetTransform()->SetPosition({-screenWidth * 0.30f, -screenHeight * 0.44f, 0.0f});
    auto instructions = instructionsText->AddComponent<TextRenderComponent>();
    instructions->SetText("W/S to move - First to 5 wins!");
    instructions->SetFontSize(static_cast<int>(screenHeight * 0.035f));

    winnerText = CreateGameObject();
    winnerText->GetTransform()->SetPosition({0.0f, 0.0f, 0.0f});
    auto winText = winnerText->AddComponent<TextRenderComponent>();
    winText->SetText("WINNER!");
    winText->SetFontSize(72);
    winText->SetColor({255, 255, 255, 255});
    winnerText->Disable();
}

void PongScene::OnSceneUpdate()
{
    if (!gameOver && (player1Score >= 5 || player2Score >= 5))
    {
        gameOver = true;
        gameOverTimer = 0.0f;

        if (winnerText)
        {
            auto winText = winnerText->GetComponent<TextRenderComponent>();
            if (winText)
            {
                if (player1Score >= 5)
                    winText->SetText("PLAYER WINS!");
                else
                    winText->SetText("AI WINS!");
            }
            winnerText->Enable();
        }

        if (ballObject)
        {
            auto ball = ballObject->GetComponent<Ball>();
            if (ball) ball->StopBall();
        }
    }

    if (gameOver)
    {
        gameOverTimer += GetContext().time->GetDeltaTime();
        if (gameOverTimer >= restartDelay)
        {
            printf("Restarting game...\n");
            ResetGame();
        }
    }
}

void PongScene::OnSceneExit()
{
    printf("Exiting Pong Scene.\n");
}

void PongScene::Player1Scored()
{
    player1Score++;
    UpdateScore();
}

void PongScene::Player2Scored()
{
    player2Score++;
    UpdateScore();
}

void PongScene::UpdateScore()
{
    if (player1ScoreText)
    {
        auto text = player1ScoreText->GetComponent<TextRenderComponent>();
        if (text) text->SetText(std::to_string(player1Score));
    }

    if (player2ScoreText)
    {
        auto text = player2ScoreText->GetComponent<TextRenderComponent>();
        if (text) text->SetText(std::to_string(player2Score));
    }
}

void PongScene::ResetGame()
{
    player1Score = 0;
    player2Score = 0;
    UpdateScore();

    gameOver = false;
    gameOverTimer = 0.0f;

    if (winnerText)
        winnerText->Disable();

    if (ballObject)
    {
        ballObject->GetTransform()->SetPosition({0.0f, 0.0f, 0.0f});
        auto ball = ballObject->GetComponent<Ball>();
        if (ball)
        {
            ball->SetInitialSpeed(280.0f);
            ball->ResetBall();
        }
    }

    printf("Game reset!\n");
}

} // namespace Axion
