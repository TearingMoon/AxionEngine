#include "MainMenuScene.hpp"

void MainMenuScene::OnSceneEnter()
{
    printf("Entered Main Menu Scene.\n");

    // Camera GameObject
    auto cameraGO = CreateGameObject();
    auto cameraComponent = cameraGO->AddComponent<CameraComponent>();
    cameraComponent->SetOrthographic(800.0f, 600.0f, -1.0f, 1.0f);
    SetCurrentCamera(cameraComponent);

    // First GameObject
    auto firstGO = CreateGameObject();
    firstGO->AddComponent<ScriptableTest>();
    firstGO->GetTransform()->SetScale({100.0f, 100.0f, 1.0f});
    auto firstGoCollider = firstGO->AddComponent<CircleColliderComponent>();
    firstGoCollider->SetRadius(1.0f);
    firstGoCollider->SetColliderType(COLLIDER_TYPE::PHYSICS);
    auto firstGoRenderer = firstGO->AddComponent<SpriteRenderComponent>();
    firstGoRenderer->SetTexture(assets().LoadTexture("TestSprite", "test.png"));
    // firstGoRenderer->SetSize({100.0f, 100.0f});

    // Delete Test GameObject
    deleteTestObject = CreateGameObject();
    deleteTestObject->AddComponent<ScriptableDeleteTest>();

    // Collider test
    auto SecondGO = CreateGameObject();
    SecondGO->GetTransform()->SetPosition({150.0f, 0.0f, 0.0f});
    SecondGO->GetTransform()->SetScale({100.0f, 100.0f, 100.0f});
    auto secondGoCollider = SecondGO->AddComponent<OBBColliderComponent>();
    secondGoCollider->SetColliderType(COLLIDER_TYPE::TRIGGER);
    secondGoCollider->SetSize({2.0f, 1.0f, 1.0f});
    auto secondGoRenderer = SecondGO->AddComponent<SquareRenderComponent>();
    secondGoRenderer->SetColor({255, 255, 255, 255});
    SecondGO->AddComponent<ScriptableRotationTest>();

    // Text GO
    auto textGO = CreateGameObject();
    textGO->GetTransform()->SetPosition({-200.0f, 200.0f, 0.0f});
    auto textRenderer = textGO->AddComponent<TextRenderComponent>();
    textRenderer->SetText("Main Menu Scene");
    textRenderer->SetFontSize(32); // Changed from 300 to a more reasonable size
    auto textRigidBody = textGO->AddComponent<RigidBody>();
    textRigidBody->AddForce(50.0f, 0.0f, 0.0f);

    // Circle Renderer Test GO
    auto circleGO = CreateGameObject();
    circleGO->GetTransform()->SetPosition({0.0f, -150.0f, 0.0f});
    circleGO->GetTransform()->SetScale({75.0f, 75.0f, 1.0f});
    auto circleRenderer = circleGO->AddComponent<CircleRenderComponent>();
    circleRenderer->SetRadius(1.0f);
    circleRenderer->SetColor({100, 255, 100, 255}); // Green color
}

void MainMenuScene::OnSceneUpdate()
{
    if (deleteFlag)
        return;

    timer += ctx_.time->GetDeltaTime();
    if (timer >= 2.0f)
    {
        deleteFlag = true;
        printf("2 seconds have passed in Main Menu Scene.\n");
        printf("Removing Delete Test GameObject\n");
        DestroyGameObject(*deleteTestObject);
    }
}