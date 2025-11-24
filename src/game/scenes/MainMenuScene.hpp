#pragma once

#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/managers/time/TimeManager.hpp"
#include "axion_engine/runtime/components/collider/SphereColliderComponent.hpp"

#include "game/scripts/ScriptableTest.hpp"

class MainMenuScene : public Scene
{
public:
    using Scene::Scene;

    void OnSceneEnter() override;

    void OnSceneUpdate() override;

    void OnSceneExit() override
    {
        printf("Exiting Main Menu Scene.\n");
    }

private:
    bool deleteFlag = false;
    float timer = 0.0f;
};