#include <catch2/catch_test_macros.hpp>

#include <cmath>

#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/components/rigid_body/RigidBody.hpp"

namespace
{
    bool NearlyEqual(float a, float b, float epsilon = 0.0001f)
    {
        return std::abs(a - b) <= epsilon;
    }
}

TEST_CASE("RigidBody integrates velocity into transform")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject object(scene);

    auto *body = object.AddComponent<Axion::RigidBody>();
    body->SetVelocity({10.0f, 5.0f, 0.0f});

    body->IntegrateVelocity(0.5f);

    const auto position = object.GetTransform()->GetPosition();

    REQUIRE(NearlyEqual(position.x, 5.0f));
    REQUIRE(NearlyEqual(position.y, 2.5f));
}

TEST_CASE("RigidBody integrates force and gravity")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject object(scene);

    auto *body = object.AddComponent<Axion::RigidBody>();
    body->mass = 2.0f;
    body->AddForce({4.0f, 0.0f, 0.0f});

    body->IntegrateForces(0.5f, {0.0f, 10.0f, 0.0f});

    const auto velocity = body->GetVelocity();

    // Force acceleration: 4 / 2 = 2, over 0.5s = 1.
    REQUIRE(NearlyEqual(velocity.x, 1.0f));

    // Gravity: 10 * 0.5 = 5.
    REQUIRE(NearlyEqual(velocity.y, 5.0f));
}

TEST_CASE("RigidBody clears accumulated force after integration")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject object(scene);

    auto *body = object.AddComponent<Axion::RigidBody>();
    body->mass = 1.0f;
    body->AddForce({10.0f, 0.0f, 0.0f});

    body->IntegrateForces(1.0f);
    body->IntegrateForces(1.0f);

    const auto velocity = body->GetVelocity();

    REQUIRE(NearlyEqual(velocity.x, 10.0f));
}