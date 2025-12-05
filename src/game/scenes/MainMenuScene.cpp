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
    auto firstGoCollider = firstGO->AddComponent<SphereColliderComponent>();
    firstGoCollider->SetRadius(5.0f);
    firstGoCollider->SetColliderType(COLLIDER_TYPE::PHYSICS);
    auto firstGoRenderer = firstGO->AddComponent<SpriteRenderComponent>();
    firstGoRenderer->SetTexture(assets().LoadTexture("TestSprite", "test.png"));
    firstGoRenderer->SetSize({100.0f, 100.0f});


    // Delete Test GameObject
    deleteTestObject = CreateGameObject();
    deleteTestObject->AddComponent<ScriptableDeleteTest>();
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