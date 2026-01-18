#pragma once

#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"
#include "axion_engine/runtime/components/renderer/SquareRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/TextRenderComponent.hpp"

class Game2Scene : public Scene
{
public:
    using Scene::Scene;

    void OnSceneEnter() override;

    void OnSceneUpdate() override;

    void OnSceneExit() override
    {
        printf("Exiting Game 2 Scene.\n");
    }
};
