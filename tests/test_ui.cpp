#include <catch2/catch_test_macros.hpp>

#include "axion_engine/runtime/ui/Canvas.hpp"

namespace
{

class FakeInput final : public Axion::IInputProvider
{
public:
    bool keyDown = false;
    bool mouseDown = false;
    bool justPressed = false;
    SDL_Point mouse{0, 0};
    bool consumed = false;

    bool IsKeyDown(SDL_Scancode) const override { return keyDown; }
    bool IsKeyJustPressed(SDL_Scancode) const override { return false; }
    bool IsKeyJustReleased(SDL_Scancode) const override { return false; }
    bool IsMouseButtonDown(Uint8) const override { return mouseDown; }
    bool IsMouseButtonJustPressed(Uint8) const override
    {
        return justPressed && !consumed;
    }
    void ConsumeMouseButton(Uint8) override { consumed = true; }
    SDL_Point GetMousePosition() const override { return mouse; }
};

} // namespace

TEST_CASE("UI anchors resolve against the viewport", "[ui]")
{
    Axion::UIRenderContext context;
    context.width = 1280;
    context.height = 720;

    Axion::UILabel label("test");
    label.SetAnchor({1.0f, 0.5f});
    label.SetPosition({-200.0f, -20.0f});
    label.SetSize({180.0f, 40.0f});

    const SDL_Rect rect = label.GetScreenRect(context);
    REQUIRE(rect.x == 1080);
    REQUIRE(rect.y == 340);
    REQUIRE(rect.w == 180);
    REQUIRE(rect.h == 40);
}

TEST_CASE("UI button reports hover and consumes its click", "[ui]")
{
    FakeInput input;
    input.mouse = {120, 80};
    input.justPressed = true;

    Axion::UIRenderContext context;
    context.width = 640;
    context.height = 480;
    context.input = &input;

    Axion::UIButton button("Restart");
    button.SetPosition({100.0f, 60.0f});
    button.SetSize({160.0f, 40.0f});

    bool clicked = false;
    button.SetOnClick([&clicked]() { clicked = true; });
    button.Update(context, {});

    REQUIRE(button.IsHovered());
    REQUIRE(clicked);
    REQUIRE(input.consumed);
    REQUIRE_FALSE(input.IsMouseButtonJustPressed(SDL_BUTTON_LEFT));
}

TEST_CASE("Disabled UI buttons do not react to input", "[ui]")
{
    FakeInput input;
    input.mouse = {120, 80};
    input.justPressed = true;

    Axion::UIRenderContext context;
    context.width = 640;
    context.height = 480;
    context.input = &input;

    Axion::UIButton button("Restart");
    button.SetPosition({100.0f, 60.0f});
    button.SetSize({160.0f, 40.0f});
    button.SetEnabled(false);

    bool clicked = false;
    button.SetOnClick([&clicked]() { clicked = true; });
    button.Update(context, {});

    REQUIRE_FALSE(button.IsHovered());
    REQUIRE_FALSE(clicked);
    REQUIRE_FALSE(input.consumed);
}
