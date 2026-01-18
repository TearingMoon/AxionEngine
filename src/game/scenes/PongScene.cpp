#include "PongScene.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"
#include <string>
#include <SDL.h>

void PongScene::OnSceneEnter()
{
    printf("Entered Pong Scene - WASD to move, AI opponent!\n");

    // Get actual window dimensions
    int windowWidth, windowHeight;
    SDL_GetWindowSize(GetContext().window->GetSDLWindow(), &windowWidth, &windowHeight);
    
    const float screenWidth = static_cast<float>(windowWidth);
    const float screenHeight = static_cast<float>(windowHeight);
    
    // Scale proportions based on screen size
    const float paddleWidth = screenWidth * 0.02f;  // 2% of screen width
    const float paddleHeight = screenHeight * 0.15f; // 15% of screen height
    const float ballSize = screenWidth * 0.02f;      // 2% of screen width
    const float paddleOffset = screenWidth * 0.42f;  // 42% from center

    // Store initial window dimensions
    lastWindowWidth = windowWidth;
    lastWindowHeight = windowHeight;
    
    // Camera GameObject
    auto cameraGO = CreateGameObject();
    camera = cameraGO->AddComponent<CameraComponent>();
    camera->SetOrthographic(screenWidth, screenHeight, -1.0f, 1.0f);
    SetCurrentCamera(camera);

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
    p1Script->SetSpeed(screenHeight * 0.7f); // Speed relative to screen height

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
    p2Script->SetSpeed(screenHeight * 0.5f); // AI slightly slower

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
    ballScript->SetInitialSpeed(screenWidth * 0.35f); // Speed relative to screen width
    
    // Set AI to follow ball
    p2Script->SetBallTransform(ballObject->GetTransform());
    
    // Set score callbacks
    ballScript->SetOnPlayer1Score([this]() { Player1Scored(); });
    ballScript->SetOnPlayer2Score([this]() { Player2Scored(); });

    // Center line (decorative) - adaptive spacing
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

    // Player 1 Score Text
    player1ScoreText = CreateGameObject();
    player1ScoreText->GetTransform()->SetPosition({-screenWidth * 0.15f, screenHeight * 0.38f, 0.0f});
    auto p1Text = player1ScoreText->AddComponent<TextRenderComponent>();
    p1Text->SetText("0");
    p1Text->SetFontSize(static_cast<int>(screenHeight * 0.12f)); // 12% of screen height

    // Player 2 Score Text
    player2ScoreText = CreateGameObject();
    player2ScoreText->GetTransform()->SetPosition({screenWidth * 0.1f, screenHeight * 0.38f, 0.0f});
    auto p2Text = player2ScoreText->AddComponent<TextRenderComponent>();
    p2Text->SetText("0");
    p2Text->SetFontSize(static_cast<int>(screenHeight * 0.12f));

    // Title Text
    auto titleText = CreateGameObject();
    titleText->GetTransform()->SetPosition({-screenWidth * 0.18f, screenHeight * 0.43f, 0.0f});
    auto title = titleText->AddComponent<TextRenderComponent>();
    title->SetText("AXION PONG");
    title->SetFontSize(static_cast<int>(screenHeight * 0.045f)); // 4.5% of screen height

    // Instructions
    auto instructionsText = CreateGameObject();
    instructionsText->GetTransform()->SetPosition({-screenWidth * 0.30f, -screenHeight * 0.44f, 0.0f});
    auto instructions = instructionsText->AddComponent<TextRenderComponent>();
    instructions->SetText("W/S to move - First to 5 wins!");
    instructions->SetFontSize(static_cast<int>(screenHeight * 0.035f)); // 3.5% of screen height
}

void PongScene::OnSceneUpdate()
{
    // Check for window resize and update camera
    CheckWindowResize();
    
    // Check win condition
    if (!gameOver && (player1Score >= 5 || player2Score >= 5))
    {
        gameOver = true;
        gameOverTimer = 0.0f;
        
        // Create winner text
        int windowWidth, windowHeight;
        SDL_GetWindowSize(GetContext().window->GetSDLWindow(), &windowWidth, &windowHeight);
        
        winnerText = CreateGameObject();
        winnerText->GetTransform()->SetPosition({-static_cast<float>(windowWidth) * 0.20f, 0.0f, 0.0f});
        auto winText = winnerText->AddComponent<TextRenderComponent>();
        
        if (player1Score >= 5)
        {
            printf("Player 1 Wins!\n");
            winText->SetText("PLAYER WINS!");
        }
        else
        {
            printf("AI Wins!\n");
            winText->SetText("AI WINS!");
        }
        winText->SetFontSize(static_cast<int>(windowHeight * 0.1f));
        
        // Pause the ball
        if (ballObject)
        {
            auto ball = ballObject->GetComponent<Ball>();
            if (ball) ball->SetInitialSpeed(0.0f);
        }
    }
    
    // Handle restart after delay
    if (gameOver)
    {
        gameOverTimer += GetContext().time->GetDeltaTime();
        if (gameOverTimer >= restartDelay)
        {
            printf("Restarting game...\n");
            GetContext().scene->RequestChange("PongScene");
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
    printf("Player 1 scores! Score: %d - %d\n", player1Score, player2Score);
}

void PongScene::Player2Scored()
{
    player2Score++;
    UpdateScore();
    printf("AI scores! Score: %d - %d\n", player1Score, player2Score);
}

void PongScene::CheckWindowResize()
{
    int currentWidth, currentHeight;
    SDL_GetWindowSize(GetContext().window->GetSDLWindow(), &currentWidth, &currentHeight);
    
    // If window size changed, update camera
    if (currentWidth != lastWindowWidth || currentHeight != lastWindowHeight)
    {
        lastWindowWidth = currentWidth;
        lastWindowHeight = currentHeight;
        
        if (camera)
        {
            camera->SetOrthographic(
                static_cast<float>(currentWidth),
                static_cast<float>(currentHeight),
                -1.0f, 1.0f
            );
        }
    }
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
