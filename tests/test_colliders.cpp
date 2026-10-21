#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <cmath>

#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/components/collider/AABBColliderComponent.hpp"
#include "axion_engine/runtime/components/collider/ColliderHelpers.hpp"
#include "axion_engine/runtime/components/collider/CircleColliderComponent.hpp"
#include "axion_engine/runtime/components/collider/OBBColliderComponent.hpp"

namespace
{
bool NearlyEqual(float a, float b, float epsilon = 0.0001f)
{
    return std::abs(a - b) <= epsilon;
}
}

TEST_CASE("Collider helpers clamp points and calculate interval overlap")
{
    REQUIRE(Axion::Clamp(-1.0f, 0.0f, 2.0f) == 0.0f);
    REQUIRE(Axion::Clamp(3.0f, 0.0f, 2.0f) == 2.0f);

    const auto closest = Axion::ClosestPointOnAABB({3.0f, -1.0f}, {0.0f, 0.0f}, {2.0f, 2.0f});
    REQUIRE(closest.x == 2.0f);
    REQUIRE(closest.y == 0.0f);

    float overlap = 0.0f;
    REQUIRE(Axion::OverlapIntervals(0.0f, 3.0f, 2.0f, 5.0f, overlap));
    REQUIRE(overlap == 1.0f);
    REQUIRE_FALSE(Axion::OverlapIntervals(0.0f, 1.0f, 1.0f, 2.0f, overlap));
    REQUIRE(Axion::DegToRad(180.0f) == Catch::Approx(3.14159265f));
}

TEST_CASE("Circle colliders report overlap, separation, and scale")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject first(scene);
    Axion::GameObject second(scene);
    auto *a = first.AddComponent<Axion::CircleColliderComponent>();
    auto *b = second.AddComponent<Axion::CircleColliderComponent>();
    a->SetRadius(2.0f);
    b->SetRadius(1.0f);
    first.GetTransform()->SetScale({2.0f, 1.0f, 1.0f});
    second.GetTransform()->SetPosition({5.5f, 0.0f, 0.0f});

    Axion::Manifold manifold;
    REQUIRE_FALSE(a->Intersects(*b, manifold));
    second.GetTransform()->SetPosition({4.5f, 0.0f, 0.0f});
    REQUIRE(a->Intersects(*b, manifold));
    REQUIRE(manifold.hit);
    REQUIRE(std::abs(manifold.normal.x) > 0.99f);
    REQUIRE(NearlyEqual(manifold.penetration, 0.5f));
    REQUIRE(NearlyEqual(a->GetRadius(), 4.0f));
}

TEST_CASE("AABB colliders use transformed size and produce a collision manifold")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject first(scene);
    Axion::GameObject second(scene);
    auto *a = first.AddComponent<Axion::AABBColliderComponent>();
    auto *b = second.AddComponent<Axion::AABBColliderComponent>();
    a->SetSize({4.0f, 2.0f, 1.0f});
    first.GetTransform()->SetScale({0.5f, 2.0f, 1.0f});
    second.GetTransform()->SetPosition({1.0f, 0.0f, 0.0f});

    REQUIRE(a->GetSize() == glm::vec3(2.0f, 4.0f, 1.0f));
    Axion::Manifold manifold;
    REQUIRE(a->Intersects(*b, manifold));
    REQUIRE(manifold.hit);
    REQUIRE(std::abs(manifold.normal.x) > 0.99f);
    REQUIRE(NearlyEqual(manifold.penetration, 0.5f));

    second.GetTransform()->SetPosition({2.0f, 0.0f, 0.0f});
    REQUIRE_FALSE(a->Intersects(*b, manifold));
}

TEST_CASE("OBB colliders detect rotated overlap and separated boxes")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject first(scene);
    Axion::GameObject second(scene);
    auto *a = first.AddComponent<Axion::OBBColliderComponent>();
    auto *b = second.AddComponent<Axion::OBBColliderComponent>();
    a->SetSize({2.0f, 2.0f, 1.0f});
    b->SetSize({2.0f, 2.0f, 1.0f});
    first.GetTransform()->SetRotation({0.0f, 0.0f, 45.0f});
    second.GetTransform()->SetPosition({1.0f, 0.0f, 0.0f});

    Axion::Manifold manifold;
    REQUIRE(a->Intersects(*b, manifold));
    REQUIRE(manifold.hit);
    REQUIRE(manifold.penetration > 0.0f);

    second.GetTransform()->SetPosition({4.0f, 0.0f, 0.0f});
    REQUIRE_FALSE(a->Intersects(*b, manifold));
    REQUIRE_FALSE(manifold.hit);
}

TEST_CASE("Collider types and shape metadata are exposed")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject object(scene);
    auto *circle = object.AddComponent<Axion::CircleColliderComponent>();

    REQUIRE(object.HasCollider());
    REQUIRE(circle->GetShape() == Axion::ColliderShape::Circle);
    REQUIRE(circle->IsTrigger());
    circle->SetColliderType(Axion::ColliderType::Physics);
    REQUIRE_FALSE(circle->IsTrigger());
}
