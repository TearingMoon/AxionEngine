#include "PongScene.hpp"
#include <string>

void PongScene::OnSceneEnter()
{
    printf("Entered Pong Scene - WASD to move, AI opponent!\n");

    const float screenWidth = 800.0f;
    const float screenHeight = 600.0f;
    const float paddleWidth = 15.0f;
    const float paddleHeight = 80.0f;
    const float ballSize = 15.0f;
    const float paddleOffset = 350.0f;

    // Camera GameObject
    auto cameraGO = CreateGameObject();
    auto cameraComponent = cameraGO->AddComponent<CameraComponent>();
    cameraComponent->SetOrthographic(screenWidth, screenHeight, -1.0f, 1.0f);
    SetCurrentCamera(cameraComponent);

    // Player 1 (Left paddle - Human)
    auto player1 = CreateGameObject();
    player1->GetTransform()->SetPosition({-paddleOffset, 0.0f, 0.0f});
    player1->GetTransform()->SetScale({paddleWidth, paddleHeight, 1.0f});
    auto p1Renderer = player1->AddComponent<SquareRenderComponent>();
    p1Renderer->SetColor({255, 255, 255, 255});
    auto p1Collider = player1->AddComponent<AABBColliderComponent>();
    p1Collider->SetColliderType(COLLIDER_TYPE::PHYSICS);
    auto p1Script = player1->AddComponent<PlayerPaddle>();
    p1Script->SetBounds(-screenHeight / 2.0f + paddleHeight / 2.0f, 
                        screenHeight / 2.0f - paddleHeight / 2.0f);
    p1Script->SetSpeed(400.0f);

    // Player 2 (Right paddle - AI)
    auto player2 = CreateGameObject();
    player2->GetTransform()->SetPosition({paddleOffset, 0.0f, 0.0f});
    player2->GetTransform()->SetScale({paddleWidth, paddleHeight, 1.0f});
    auto p2Renderer = player2->AddComponent<SquareRenderComponent>();
    p2Renderer->SetColor({255, 255, 255, 255});
    auto p2Collider = player2->AddComponent<AABBColliderComponent>();
    p2Collider->SetColliderType(COLLIDER_TYPE::PHYSICS);
    auto p2Script = player2->AddComponent<AIPaddle>();
    p2Script->SetBounds(-screenHeight / 2.0f + paddleHeight / 2.0f, 
                        screenHeight / 2.0f - paddleHeight / 2.0f);
    p2Script->SetSpeed(300.0f);

    // Ball
    ballObject = CreateGameObject();
    ballObject->GetTransform()->SetPosition({0.0f, 0.0f, 0.0f});
    ballObject->GetTransform()->SetScale({ballSize, ballSize, 1.0f});
    auto ballRenderer = ballObject->AddComponent<SquareRenderComponent>();
    ballRenderer->SetColor({255, 255, 100, 255}); // Yellow
    auto ballCollider = ballObject->AddComponent<AABBColliderComponent>();
    ballCollider->SetColliderType(COLLIDER_TYPE::PHYSICS);
    auto ballScript = ballObject->AddComponent<Ball>();
    ballScript->SetBounds(-screenWidth / 2.0f, screenWidth / 2.0f, 
                          -screenHeight / 2.0f + ballSize, screenHeight / 2.0f - ballSize);
    
    // Set AI to follow ball
    p2Script->SetBallTransform(ballObject->GetTransform());
    
    // Set score callbacks
    ballScript->SetOnPlayer1Score([this]() { Player1Scored(); });
    ballScript->SetOnPlayer2Score([this]() { Player2Scored(); });

    // Center line (decorative)
    for (int i = 0; i < 15; i++)
    {
        auto line = CreateGameObject();
        line->GetTransform()->SetPosition({0.0f, -screenHeight / 2.0f + i * 45.0f, 0.0f});
        line->GetTransform()->SetScale({5.0f, 30.0f, 1.0f});
        auto lineRenderer = line->AddComponent<SquareRenderComponent>();
        lineRenderer->SetColor({100, 100, 100, 255});
    }

    // Player 1 Score Text
    player1ScoreText = CreateGameObject();
    player1ScoreText->GetTransform()->SetPosition({-100.0f, 250.0f, 0.0f});
    auto p1Text = player1ScoreText->AddComponent<TextRenderComponent>();
    p1Text->SetText("0");
    p1Text->SetFontSize(64);

    // Player 2 Score Text
    player2ScoreText = CreateGameObject();
    player2ScoreText->GetTransform()->SetPosition({100.0f, 250.0f, 0.0f});
    auto p2Text = player2ScoreText->AddComponent<TextRenderComponent>();
    p2Text->SetText("0");
    p2Text->SetFontSize(64);

    // Title Text
    auto titleText = CreateGameObject();
    titleText->GetTransform()->SetPosition({-150.0f, 270.0f, 0.0f});
    auto title = titleText->AddComponent<TextRenderComponent>();
    title->SetText("AXION PONG");
    title->SetFontSize(24);

    // Instructions
    auto instructionsText = CreateGameObject();
    instructionsText->GetTransform()->SetPosition({-200.0f, -270.0f, 0.0f});
    auto instructions = instructionsText->AddComponent<TextRenderComponent>();
    instructions->SetText("W/S to move - First to 5 wins!");
    instructions->SetFontSize(20);
}

void PongScene::OnSceneUpdate()
{
    // Game logic is handled in the scripts
    // Could add win condition check here
    if (player1Score >= 5)
    {
        printf("Player 1 Wins!\n");
        // Could reset or change scene
    }
    else if (player2Score >= 5)
    {
        printf("AI Wins!\n");
        // Could reset or change scene
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
    printf("Player 1 scores! Score: %d - %d\n", player1Score, player2Score);
}

void PongScene::Player2Scored()
{
    player2Score++;
    UpdateScore();
    printf("AI scores! Score: %d - %d\n", player1Score, player2Score);
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
