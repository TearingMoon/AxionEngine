#pragma once

#include "axion_engine/Axion.hpp"

#include "game_debug/scripts/ScriptableTest.hpp"
#include "game_debug/scripts/ScriptableDeleteTest.hpp"
#include "game_debug/scripts/ScriptableRotationTest.hpp"

namespace Axion
{

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

    GameObject *deleteTestObject = nullptr;
};

} // namespace Axion