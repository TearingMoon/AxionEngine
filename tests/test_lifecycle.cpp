#include <catch2/catch_test_macros.hpp>

#include "axion_engine/managers/scene/SceneManager.hpp"
#include "axion_engine/managers/logger/Logger.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/interfaces/IUpdateable.hpp"

namespace
{
class LifecycleComponent : public Axion::Component, public Axion::IUpdateable
{
public:
    int mounted = 0;
    int enabled = 0;
    int disabled = 0;
    int destroyed = 0;
    int starts = 0;
    int updates = 0;
    int lateUpdates = 0;

    void OnMounted(Axion::EngineContext&) override { ++mounted; }
    void OnEnabled(Axion::EngineContext&) override { ++enabled; }
    void OnDisabled(Axion::EngineContext&) override { ++disabled; }
    void OnDestroy(Axion::EngineContext&) override { ++destroyed; }
    void Start(Axion::EngineContext&) override { ++starts; }
    void Update(Axion::EngineContext&) override { ++updates; }
    void LateUpdate(Axion::EngineContext&) override { ++lateUpdates; }
};

class SpawnScene : public Axion::Scene
{
public:
    using Axion::Scene::Scene;
    Axion::GameObject *created = nullptr;

protected:
    void OnSceneEnter() override
    {
        created = CreateGameObject();
        created->AddComponent<LifecycleComponent>();
    }
};
}

TEST_CASE("GameObject lifecycle is idempotent and dynamic components are mounted")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject object(scene);

    auto *initial = object.AddComponent<LifecycleComponent>();
    object.Mounted();
    object.Mounted();
    REQUIRE(initial->mounted == 1);
    REQUIRE(initial->enabled == 1);

    object.Tick();
    object.Tick();
    REQUIRE(initial->starts == 1);
    REQUIRE(initial->updates == 1);

    auto *dynamic = object.AddComponent<LifecycleComponent>();
    REQUIRE(dynamic->mounted == 1);
    REQUIRE(dynamic->enabled == 1);
    REQUIRE(dynamic->starts == 1);

    object.Tick();
    object.LateUpdate();
    REQUIRE(initial->updates == 2);
    REQUIRE(dynamic->updates == 1);
    REQUIRE(initial->lateUpdates == 1);
    REQUIRE(dynamic->lateUpdates == 1);

    object.Disable();
    object.Disable();
    REQUIRE(initial->disabled == 1);
    REQUIRE(dynamic->disabled == 1);
    object.Enable();
    REQUIRE(initial->enabled == 2);
    REQUIRE(dynamic->enabled == 2);

    object.OnDestroy();
    object.OnDestroy();
    REQUIRE(initial->destroyed == 1);
    REQUIRE(dynamic->destroyed == 1);
}

TEST_CASE("SceneManager enters scenes and mounts objects spawned during scene entry")
{
    Axion::EngineContext context{};
    Axion::Logger logger;
    context.logger = &logger;
    Axion::SceneManager manager(context);
    context.sceneProvider = &manager;
    manager.RegisterScene<SpawnScene>("spawn");

    manager.RequestChange("spawn");
    manager.ProcessRequests();

    auto *scene = dynamic_cast<SpawnScene *>(manager.GetCurrentScene());
    REQUIRE(scene != nullptr);
    REQUIRE(scene->created != nullptr);

    size_t objectCount = 0;
    for (auto *object : scene->GetGameObjects())
    {
        REQUIRE(object != nullptr);
        ++objectCount;
    }
    REQUIRE(objectCount == 1);
    REQUIRE(scene->created->GetComponent<LifecycleComponent>()->mounted == 1);
}

TEST_CASE("Destroying a GameObject queues it and invokes component destruction once")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    auto *object = scene.CreateGameObject();
    auto *component = object->AddComponent<LifecycleComponent>();
    object->Mounted();

    object->Destroy();
    object->Destroy();
    REQUIRE(object->IsDestroyed());
    object->OnDestroy();
    object->OnDestroy();
    REQUIRE(component->destroyed == 1);
}
