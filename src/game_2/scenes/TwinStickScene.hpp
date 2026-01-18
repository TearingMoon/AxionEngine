#pragma once

#include "axion_engine/managers/assets/AssetsManager.hpp"

#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"
#include "axion_engine/runtime/components/renderer/SquareRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/CircleRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/TextRenderComponent.hpp"
#include "axion_engine/runtime/components/renderer/SpriteRenderComponent.hpp"

#include "axion_engine/runtime/components/collider/CircleColliderComponent.hpp"

#include "game_2/scripts/ReticleScript.hpp"
#include "game_2/scripts/PlayerScript.hpp"
#include "game_2/scripts/ZombieScript.hpp"
#include "game_2/scripts/BulletScript.hpp"
#include "game_2/scripts/CameraFollowScript.hpp"

class TwinStickScene : public Scene
{
public:
    using Scene::Scene;

    void OnSceneEnter() override;

    void OnSceneUpdate() override;

    void OnSceneExit() override
    {
        printf("Exiting Twin Stick Scene.\n");
    }

private:
    GameObject *player = nullptr;
    float zombieSpawnTimer = 0.0f;
    float zombieSpawnInterval = 2.0f;
    int zombiesSpawned = 0;
    int maxZombies = 20;

    void SpawnZombie();
};
