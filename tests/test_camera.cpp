#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "axion_engine/runtime/classes/gameObject/GameObject.hpp"
#include "axion_engine/runtime/classes/scene/Scene.hpp"
#include "axion_engine/runtime/components/camera/CameraComponent.hpp"

TEST_CASE("Camera maps orthographic world coordinates to the viewport")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject cameraObject(scene);
    auto* camera = cameraObject.AddComponent<Axion::CameraComponent>();
    camera->SetOrthographic(100.0f, 50.0f, -1.0f, 1.0f);

    const auto center = camera->WorldToScreen({0.0f, 0.0f, 0.0f}, 1000, 500);
    const auto topRight = camera->WorldToScreen({50.0f, 25.0f, 0.0f}, 1000, 500);

    REQUIRE(center.x == 500.0f);
    REQUIRE(center.y == 250.0f);
    REQUIRE(topRight.x == 1000.0f);
    REQUIRE(topRight.y == 0.0f);
}

TEST_CASE("Camera follows its transform when converting world coordinates")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject cameraObject(scene);
    auto* camera = cameraObject.AddComponent<Axion::CameraComponent>();
    camera->SetOrthographic(100.0f, 50.0f, -1.0f, 1.0f);
    cameraObject.GetTransform()->SetPosition({10.0f, -5.0f, 0.0f});

    const auto screen = camera->WorldToScreen({10.0f, -5.0f, 0.0f}, 1000, 500);

    REQUIRE(screen.x == 500.0f);
    REQUIRE(screen.y == 250.0f);
}

TEST_CASE("Camera roll rotates the screen-space axes")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject cameraObject(scene);
    auto* camera = cameraObject.AddComponent<Axion::CameraComponent>();
    camera->SetOrthographic(100.0f, 50.0f, -100.0f, 100.0f);
    cameraObject.GetTransform()->SetRotation({0.0f, 0.0f, 90.0f});

    const auto screen = camera->WorldToScreen({0.0f, 10.0f, 0.0f}, 1000, 500);

    REQUIRE(screen.x == Catch::Approx(600.0f));
    REQUIRE(screen.y == Catch::Approx(250.0f));
}

TEST_CASE("Camera yaw changes which world depth is visible")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject cameraObject(scene);
    auto* camera = cameraObject.AddComponent<Axion::CameraComponent>();
    camera->SetPerspective(90.0f, 2.0f, 0.1f, 100.0f);
    cameraObject.GetTransform()->SetRotation({0.0f, 90.0f, 0.0f});

    const glm::mat4 view = camera->GetViewMatrix();
    const glm::vec4 cameraSpacePoint =
        view * glm::vec4(-10.0f, 0.0f, 0.0f, 1.0f);

    REQUIRE(cameraSpacePoint.z == Catch::Approx(-10.0f));
    REQUIRE(cameraSpacePoint.x == Catch::Approx(0.0f).margin(0.001f));
}

TEST_CASE("Camera converts world sizes using the configured projection")
{
    Axion::EngineContext context{};
    Axion::Scene scene(context);
    Axion::GameObject cameraObject(scene);
    auto* camera = cameraObject.AddComponent<Axion::CameraComponent>();
    camera->SetOrthographic(100.0f, 50.0f, -1.0f, 1.0f);

    const auto size = camera->WorldSizeToScreen(
        {0.0f, 0.0f, 0.0f}, {10.0f, 10.0f}, 1000, 500);

    REQUIRE(size.x == 100.0f);
    REQUIRE(size.y == 100.0f);
}
