#include <catch2/catch_test_macros.hpp>

#include <memory>

#include "axion_engine/managers/logger/Logger.hpp"
#include "axion_engine/managers/physics/PhysicsManager.hpp"
#include "axion_engine/managers/scene/SceneManager.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/components/collider/CircleColliderComponent.hpp"
#include "axion_engine/runtime/interfaces/ICollisionListener.hpp"

namespace
{
class CollisionRecorder final : public Axion::Component,
                                 public Axion::ICollisionListener
{
public:
    int collisionEnter = 0;
    int collisionExit = 0;
    int triggerEnter = 0;
    int triggerExit = 0;

    void OnCollisionEnter(Axion::GameObject&) override { ++collisionEnter; }
    void OnCollisionExit(Axion::GameObject&) override { ++collisionExit; }
    void OnTriggerEnter(Axion::GameObject&) override { ++triggerEnter; }
    void OnTriggerExit(Axion::GameObject&) override { ++triggerExit; }
};

class PhysicsScene final : public Axion::Scene
{
public:
    using Axion::Scene::Scene;

    Axion::GameObject* first = nullptr;
    Axion::GameObject* second = nullptr;

protected:
    void OnSceneEnter() override
    {
        first = CreateGameObject();
        first->AddComponent<Axion::CircleColliderComponent>()->SetRadius(2.0f);
        first->AddComponent<CollisionRecorder>();

        second = CreateGameObject();
        second->GetTransform()->SetPosition({2.0f, 0.0f, 0.0f});
        second->AddComponent<Axion::CircleColliderComponent>()->SetRadius(2.0f);
        second->AddComponent<CollisionRecorder>();
    }
};

struct PhysicsFixture
{
    Axion::EngineContext context{};
    Axion::Logger logger;
    std::unique_ptr<Axion::SceneManager> scenes;
    std::unique_ptr<Axion::PhysicsManager> physics;

    PhysicsFixture()
    {
        context.logger = &logger;
        scenes = std::make_unique<Axion::SceneManager>(context);
        context.sceneProvider = scenes.get();
        physics = std::make_unique<Axion::PhysicsManager>(context);
        scenes->RegisterScene<PhysicsScene>("physics");
        scenes->RequestChange("physics");
        scenes->ProcessRequests();
    }

    PhysicsScene& scene()
    {
        return *static_cast<PhysicsScene*>(scenes->GetCurrentScene());
    }
};
}

TEST_CASE("Physics dispatches collision enter once and exit after separation")
{
    PhysicsFixture fixture;
    auto& scene = fixture.scene();
    auto* firstRecorder = scene.first->GetComponent<CollisionRecorder>();
    auto* secondRecorder = scene.second->GetComponent<CollisionRecorder>();
    scene.first->GetComponent<Axion::CircleColliderComponent>()
        ->SetColliderType(Axion::ColliderType::Physics);
    scene.second->GetComponent<Axion::CircleColliderComponent>()
        ->SetColliderType(Axion::ColliderType::Physics);

    fixture.physics->Update(0.02f);
    fixture.physics->Update(0.02f);

    REQUIRE(firstRecorder->collisionEnter == 1);
    REQUIRE(secondRecorder->collisionEnter == 1);
    REQUIRE(firstRecorder->collisionExit == 0);

    scene.second->GetTransform()->SetPosition({10.0f, 0.0f, 0.0f});
    fixture.physics->Update(0.02f);

    REQUIRE(firstRecorder->collisionExit == 1);
    REQUIRE(secondRecorder->collisionExit == 1);
}

TEST_CASE("Physics dispatches trigger events without physical collision resolution")
{
    PhysicsFixture fixture;
    auto& scene = fixture.scene();
    auto* firstCollider =
        scene.first->GetComponent<Axion::CircleColliderComponent>();
    auto* secondCollider =
        scene.second->GetComponent<Axion::CircleColliderComponent>();
    auto* firstRecorder = scene.first->GetComponent<CollisionRecorder>();
    auto* secondRecorder = scene.second->GetComponent<CollisionRecorder>();

    firstCollider->SetColliderType(Axion::ColliderType::Trigger);
    secondCollider->SetColliderType(Axion::ColliderType::Physics);

    fixture.physics->Update(0.02f);

    REQUIRE(firstRecorder->triggerEnter == 1);
    REQUIRE(secondRecorder->triggerEnter == 1);
    REQUIRE(firstRecorder->collisionEnter == 0);
    REQUIRE(secondRecorder->collisionEnter == 0);

    scene.second->GetTransform()->SetPosition({10.0f, 0.0f, 0.0f});
    fixture.physics->Update(0.02f);

    REQUIRE(firstRecorder->triggerExit == 1);
    REQUIRE(secondRecorder->triggerExit == 1);
}
