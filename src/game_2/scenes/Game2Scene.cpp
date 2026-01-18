#include "Game2Scene.hpp"

void Game2Scene::OnSceneEnter()
{
    printf("Entered Game 2 Scene.\n");

    // Camera GameObject
    auto cameraGO = CreateGameObject();
    auto cameraComponent = cameraGO->AddComponent<CameraComponent>();
    cameraComponent->SetOrthographic(1024.0f, 768.0f, -1.0f, 1.0f);
    SetCurrentCamera(cameraComponent);

    // Example Square GameObject
    auto squareGO = CreateGameObject();
    squareGO->GetTransform()->SetPosition({0.0f, 0.0f, 0.0f});
    squareGO->GetTransform()->SetScale({100.0f, 100.0f, 1.0f});
    auto squareRenderer = squareGO->AddComponent<SquareRenderComponent>();
    squareRenderer->SetColor({100, 200, 255, 255});

    // Welcome Text GameObject
    auto textGO = CreateGameObject();
    textGO->GetTransform()->SetPosition({-300.0f, 250.0f, 0.0f});
    auto textRenderer = textGO->AddComponent<TextRenderComponent>();
    textRenderer->SetText("Welcome to Game 2!");
    textRenderer->SetFontSize(48);
}

void Game2Scene::OnSceneUpdate()
{
    // Add your game logic here
}
