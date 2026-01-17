#pragma once

#include "ColliderComponent.hpp"

class SphereColliderComponent : public ColliderComponent
{
public:
    SphereColliderComponent() = default;
    void SetRadius(float radius) { radius_ = radius; }
    float GetRadius() const { return radius_ * GetOwner()->GetTransform()->GetScale().x; }

    int GetLayer() const override { return 0; }
    int GetOrderInLayer() const override { return 0; }
    int GetSortKey() const override { return 0; }

    void Render(const RenderContext &ctx) override
    {
        auto *transform = GetOwner()->GetTransform();
        if (!transform || !ctx.camera || !ctx.sdl)
            return;

        const glm::vec3 worldPos = transform->GetWorldPosition();
        const glm::vec3 camWorldPos = ctx.camera->GetOwner()->GetTransform()->GetWorldPosition();

        auto WorldToScreen2D = [&](const glm::vec3 &p) -> glm::vec2
        {
            const float x = (p.x - camWorldPos.x) * 1.0f + ctx.windowWidth * 0.5f;
            const float y = ctx.windowHeight * 0.5f - (p.y - camWorldPos.y) * 1.0f; // invert Y
            return {x, y};
        };

        const glm::vec2 screenPos = WorldToScreen2D(worldPos);

        // GetRadius() already applies scale
        const float worldRadius = GetRadius();
        const glm::vec3 radiusWorldPoint = worldPos + glm::vec3(worldRadius, 0.0f, 0.0f);

        const glm::vec2 radiusScreenPoint = WorldToScreen2D(radiusWorldPoint);

        int screenRadius = (int)glm::abs(radiusScreenPoint.x - screenPos.x);
        if (screenRadius < 1)
            screenRadius = 1;

        if (IsTrigger())
            SDL_SetRenderDrawColor(ctx.sdl, 0, 255, 255, 255);
        else
            SDL_SetRenderDrawColor(ctx.sdl, 0, 255, 0, 255);

        DrawCircle(ctx, (int)screenPos.x, (int)screenPos.y, screenRadius);

        SDL_SetRenderDrawColor(ctx.sdl, 255, 0, 0, 255);
        SDL_RenderDrawPoint(ctx.sdl, (int)screenPos.x, (int)screenPos.y);
    }

    // TODO: Move to other utility class
    void DrawCircle(const RenderContext &ctx, int centerX, int centerY, int radius)
    {
        int x = 0;
        int y = radius;
        int d = 3 - 2 * radius;

        auto drawCirclePoints = [&](int cx, int cy, int x, int y)
        {
            SDL_RenderDrawPoint(ctx.sdl, cx + x, cy + y);
            SDL_RenderDrawPoint(ctx.sdl, cx - x, cy + y);
            SDL_RenderDrawPoint(ctx.sdl, cx + x, cy - y);
            SDL_RenderDrawPoint(ctx.sdl, cx - x, cy - y);
            SDL_RenderDrawPoint(ctx.sdl, cx + y, cy + x);
            SDL_RenderDrawPoint(ctx.sdl, cx - y, cy + x);
            SDL_RenderDrawPoint(ctx.sdl, cx + y, cy - x);
            SDL_RenderDrawPoint(ctx.sdl, cx - y, cy - x);
        };

        while (y >= x)
        {
            drawCirclePoints(centerX, centerY, x, y);
            x++;

            if (d > 0)
            {
                y--;
                d = d + 4 * (x - y) + 10;
            }
            else
            {
                d = d + 4 * x + 6;
            }
        }
    }

private:
    float radius_ = 1.0f;
};