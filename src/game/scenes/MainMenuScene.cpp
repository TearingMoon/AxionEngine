#include "MainMenuScene.hpp"

void MainMenuScene::OnSceneEnter()
{
    printf("Entered Main Menu Scene.\n");

    GameObject *firstGO = CreateGameObject();
    firstGO->AddComponent<ScriptableTest>();
    firstGO->AddComponent<SphereColliderComponent>();
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
        printf("Removing all GameObjects from the scene.\n");

        for (auto obj : GetGameObjects())
        {
            DestroyGameObject(*obj);
        }
    }
}