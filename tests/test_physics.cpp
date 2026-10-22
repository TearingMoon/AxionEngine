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

class ResolutionScene final : public Axion::Scene
{
public:
    using Axion::Scene::Scene;

    Axion::GameObject* staticObject = nullptr;
    Axion::GameObject* dynamicObject = nullptr;

protected:
    void OnSceneEnter() override
    {
        SetGravity({0.0f, 0.0f, 0.0f});

        staticObject = CreateGameObject();
        auto* staticCollider =
            staticObject->AddComponent<Axion::CircleColliderComponent>();
        staticCollider->SetRadius(1.0f);
        staticCollider->SetColliderType(Axion::ColliderType::Physics);
        auto* staticBody = staticObject->AddComponent<Axion::RigidBody>();
        staticBody->SetStatic(true);

        dynamicObject = CreateGameObject();
        dynamicObject->GetTransform()->SetPosition({1.5f, 0.0f, 0.0f});
        auto* dynamicCollider =
            dynamicObject->AddComponent<Axion::CircleColliderComponent>();
        dynamicCollider->SetRadius(1.0f);
        dynamicCollider->SetColliderType(Axion::ColliderType::Physics);
        dynamicObject->AddComponent<Axion::RigidBody>();
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

struct ResolutionFixture
{
    Axion::EngineContext context{};
    Axion::Logger logger;
    std::unique_ptr<Axion::SceneManager> scenes;
    std::unique_ptr<Axion::PhysicsManager> physics;

    ResolutionFixture()
    {
        context.logger = &logger;
        scenes = std::make_unique<Axion::SceneManager>(context);
        context.sceneProvider = scenes.get();
        physics = std::make_unique<Axion::PhysicsManager>(context);
        scenes->RegisterScene<ResolutionScene>("resolution");
        scenes->RequestChange("resolution");
        scenes->ProcessRequests();
    }

    ResolutionScene& scene()
    {
        return *static_cast<ResolutionScene*>(scenes->GetCurrentScene());
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

TEST_CASE("Physics separates an approaching dynamic body from a static body")
{
    ResolutionFixture fixture;
    auto& scene = fixture.scene();
    auto* body = scene.dynamicObject->GetComponent<Axion::RigidBody>();
    body->SetVelocity({-10.0f, 0.0f, 0.0f});

    const glm::vec3 start =
        scene.dynamicObject->GetTransform()->GetPosition();

    fixture.physics->Update(0.02f);

    const glm::vec3 end =
        scene.dynamicObject->GetTransform()->GetPosition();
    REQUIRE(end.x > start.x);
}

TEST_CASE("Physics treats static bodies as immovable during resolution")
{
    ResolutionFixture fixture;
    auto& scene = fixture.scene();
    const glm::vec3 staticStart =
        scene.staticObject->GetTransform()->GetPosition();
    const glm::vec3 dynamicStart =
        scene.dynamicObject->GetTransform()->GetPosition();

    fixture.physics->Update(0.02f);

    const glm::vec3 staticEnd =
        scene.staticObject->GetTransform()->GetPosition();
    const glm::vec3 dynamicEnd =
        scene.dynamicObject->GetTransform()->GetPosition();

    REQUIRE(staticEnd == staticStart);
    REQUIRE(dynamicEnd.x > dynamicStart.x);
}

TEST_CASE("Physics locks prevent collision correction on the locked axis")
{
    ResolutionFixture fixture;
    auto& scene = fixture.scene();
    auto* body = scene.dynamicObject->GetComponent<Axion::RigidBody>();
    body->SetLocks(Axion::Lock_PosX);

    const glm::vec3 start =
        scene.dynamicObject->GetTransform()->GetPosition();

    fixture.physics->Update(0.02f);

    const glm::vec3 end =
        scene.dynamicObject->GetTransform()->GetPosition();
    REQUIRE(end.x == start.x);
}
