#pragma once

#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/managers/time/TimeManager.hpp"
#include "axion_engine/managers/assets/AssetsManager.hpp"
#include "axion_engine/runtime/components/collider/CircleColliderComponent.hpp"
#include "axion_engine/runtime/components/collider/OBBColliderComponent.hpp"
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"
#include "axion_engine/runtime/components/renderer/SpriteRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/SquareRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/TextRenderComponent.hpp"

#include "axion_engine/runtime/components/rigid_body/RigidBody.hpp"

#include "game/scripts/ScriptableTest.hpp"
#include "game/scripts/ScriptableDeleteTest.hpp"
#include "game/scripts/ScriptableRotationTest.hpp"

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